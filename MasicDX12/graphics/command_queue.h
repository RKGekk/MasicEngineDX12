#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>

#include "../tools/thread_safe_queue.h"

class CommandList;
class Device;

class CommandQueue {
public:
	using FenceValueType = uint64_t;

	std::shared_ptr<CommandList> GetCommandList();

	FenceValueType ExecuteCommandList(std::shared_ptr<CommandList> command_lList);
	FenceValueType ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& command_lists);

	FenceValueType Signal();
	bool IsFenceComplete(FenceValueType fenceValue);
	void WaitForFenceValue(FenceValueType fenceValue);
	void Flush();

	void Wait(const CommandQueue& other);

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

	CommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandQueue();

private:
	void ProccessInFlightCommandLists();

	using CommandListEntry = std::tuple<FenceValueType, std::shared_ptr<CommandList>>;

	Device& m_device;
	D3D12_COMMAND_LIST_TYPE m_command_list_type;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12_command_queue;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12_fence;
	std::atomic_uint64_t m_fence_value;

	ThreadSafeQueue<CommandListEntry> m_in_flight_command_lists;
	ThreadSafeQueue<std::shared_ptr<CommandList>> m_available_command_lists;

	std::thread m_process_in_flight_command_lists_thread;
	std::atomic_bool m_bProcess_in_flight_command_lists;
	std::mutex m_process_in_flight_command_lists_thread_mutex;
	std::condition_variable m_process_in_flight_command_lists_thread_cv;
};