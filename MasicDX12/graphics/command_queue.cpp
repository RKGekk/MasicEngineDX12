#include "command_queue.h"

#include "../tools/com_exception.h"
#include "../tools/thread_utility.h"

#include <cassert>

#include "command_list.h"
#include "device.h"
#include "resource_state_tracker.h"

class MakeCommandList : public CommandList {
public:
    MakeCommandList(Device& device, D3D12_COMMAND_LIST_TYPE type) : CommandList(device, type) {}

    virtual ~MakeCommandList() {}
};

CommandQueue::CommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type) : m_device(device), m_command_list_type(type), m_fence_value(0), m_bProcess_in_flight_command_lists(true) {
    auto d3d12_device = m_device.GetD3D12Device();

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    HRESULT hr = d3d12_device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_d3d12_command_queue.GetAddressOf()));
    ThrowIfFailed(hr);

    hr = d3d12_device->CreateFence(m_fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_d3d12_fence.GetAddressOf()));
    ThrowIfFailed(hr);

    switch (type) {
        case D3D12_COMMAND_LIST_TYPE_COPY:
            m_d3d12_command_queue->SetName(L"Copy Command Queue");
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            m_d3d12_command_queue->SetName(L"Compute Command Queue");
            break;
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            m_d3d12_command_queue->SetName(L"Direct Command Queue");
            break;
    }

    char thread_name[256];
    sprintf_s(thread_name, "ProccessInFlightCommandLists ");
    switch (type) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            strcat_s(thread_name, "(Direct)");
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            strcat_s(thread_name, "(Compute)");
            break;
        case D3D12_COMMAND_LIST_TYPE_COPY:
            strcat_s(thread_name, "(Copy)");
            break;
        default:
            break;
    }

    m_process_in_flight_command_lists_thread = std::thread(&CommandQueue::ProccessInFlightCommandLists, this);
    SetThreadName(m_process_in_flight_command_lists_thread, thread_name);
}

CommandQueue::~CommandQueue() {
    m_bProcess_in_flight_command_lists = false;
    m_process_in_flight_command_lists_thread.join();
}

uint64_t CommandQueue::Signal() {
    uint64_t fence_value = ++m_fence_value;
    m_d3d12_command_queue->Signal(m_d3d12_fence.Get(), fence_value);
    return fence_value;
}

bool CommandQueue::IsFenceComplete(FenceValueType fenceValue) {
    return m_d3d12_fence->GetCompletedValue() >= fenceValue;
}

void CommandQueue::WaitForFenceValue(FenceValueType fence_value) {
    if (!IsFenceComplete(fence_value)) {
        auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        if (event) {
            m_d3d12_fence->SetEventOnCompletion(fence_value, event);
            ::WaitForSingleObject(event, DWORD_MAX);
            ::CloseHandle(event);
        }
    }
}

void CommandQueue::Flush() {
    std::unique_lock<std::mutex> lock(m_process_in_flight_command_lists_thread_mutex);
    m_process_in_flight_command_lists_thread_cv.wait(lock, [this] { return m_in_flight_command_lists.Empty(); });

    WaitForFenceValue(m_fence_value);
}

std::shared_ptr<CommandList> CommandQueue::GetCommandList() {
    std::shared_ptr<CommandList> command_list;

    if (!m_available_command_lists.Empty()) {
        m_available_command_lists.TryPop(command_list);
    }
    else {
        command_list = std::make_shared<MakeCommandList>(m_device, m_command_list_type);
    }

    return command_list;
}

uint64_t CommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> command_list) {
    return ExecuteCommandLists(std::vector<std::shared_ptr<CommandList>>({ command_list }));
}

uint64_t CommandQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& command_lists) {
    ResourceStateTracker::Lock();

    std::vector<std::shared_ptr<CommandList>> to_be_queued;
    to_be_queued.reserve(command_lists.size() * 2u);

    std::vector<std::shared_ptr<CommandList>> generate_mips_command_lists;
    generate_mips_command_lists.reserve(command_lists.size());

    std::vector<ID3D12CommandList*> d3d12_command_lists;
    d3d12_command_lists.reserve(command_lists.size() * 2u);

    for (auto command_list : command_lists) {
        auto pending_command_list = GetCommandList();
        bool has_pending_barriers = command_list->Close(pending_command_list);
        pending_command_list->Close();

        if (has_pending_barriers) {
            d3d12_command_lists.push_back(pending_command_list->GetD3D12CommandList().Get());
        }
        d3d12_command_lists.push_back(command_list->GetD3D12CommandList().Get());

        to_be_queued.push_back(pending_command_list);
        to_be_queued.push_back(command_list);

        auto generate_mips_command_list = command_list->GetGenerateMipsCommandList();
        if (generate_mips_command_list) {
            generate_mips_command_lists.push_back(generate_mips_command_list);
        }
    }

    UINT num_command_lists = static_cast<UINT>(d3d12_command_lists.size());
    m_d3d12_command_queue->ExecuteCommandLists(num_command_lists, d3d12_command_lists.data());
    uint64_t fence_value = Signal();

    ResourceStateTracker::Unlock();

    for (auto command_list : to_be_queued) {
        m_in_flight_command_lists.Push({ fence_value, command_list });
    }

    if (generate_mips_command_lists.size() > 0) {
        auto& computeQueue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
        computeQueue.Wait(*this);
        computeQueue.ExecuteCommandLists(generate_mips_command_lists);
    }

    return fence_value;
}

void CommandQueue::Wait(const CommandQueue& other) {
    m_d3d12_command_queue->Wait(other.m_d3d12_fence.Get(), other.m_fence_value);
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const {
    return m_d3d12_command_queue;
}

void CommandQueue::ProccessInFlightCommandLists() {
    std::unique_lock<std::mutex> lock(m_process_in_flight_command_lists_thread_mutex, std::defer_lock);

    while (m_bProcess_in_flight_command_lists) {
        CommandListEntry command_list_entry;

        lock.lock();
        while (m_in_flight_command_lists.TryPop(command_list_entry)) {
            auto fence_value = std::get<0>(command_list_entry);
            auto command_list = std::get<1>(command_list_entry);

            WaitForFenceValue(fence_value);

            command_list->Reset();

            m_available_command_lists.Push(command_list);
        }
        lock.unlock();
        m_process_in_flight_command_lists_thread_cv.notify_one();

        std::this_thread::yield();
    }
}