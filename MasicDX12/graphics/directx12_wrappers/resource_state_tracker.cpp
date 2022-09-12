#include "resource_state_tracker.h"

#include "command_list.h"
#include "resource.h"

#include <directx/d3dx12.h>

std::mutex ResourceStateTracker::ms_global_mutex;
bool ResourceStateTracker::ms_is_locked = false;
ResourceStateTracker::ResourceStateMap ResourceStateTracker::ms_global_resource_state;

ResourceStateTracker::ResourceStateTracker() {}

ResourceStateTracker::~ResourceStateTracker() {}

void ResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
    if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
        const D3D12_RESOURCE_TRANSITION_BARRIER& transition_barrier = barrier.Transition;

        const auto iter = m_final_resource_state.find(transition_barrier.pResource);
        if (iter != m_final_resource_state.end()) {
            auto& resource_state = iter->second;
            if (transition_barrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resource_state.SubresourceState.empty()) {
                for (auto subresource_state : resource_state.SubresourceState) {
                    if (transition_barrier.StateAfter != subresource_state.second) {
                        D3D12_RESOURCE_BARRIER new_barrier = barrier;
                        new_barrier.Transition.Subresource = subresource_state.first;
                        new_barrier.Transition.StateBefore = subresource_state.second;
                        m_resource_barriers.push_back(new_barrier);
                    }
                }
            }
            else {
                auto final_state = resource_state.GetSubresourceState(transition_barrier.Subresource);
                if (transition_barrier.StateAfter != final_state) {
                    D3D12_RESOURCE_BARRIER new_barrier = barrier;
                    new_barrier.Transition.StateBefore = final_state;
                    m_resource_barriers.push_back(new_barrier);
                }
            }
        }
        else {
            m_pending_resource_barriers.push_back(barrier);
        }

        m_final_resource_state[transition_barrier.pResource].SetSubresourceState(transition_barrier.Subresource, transition_barrier.StateAfter);
    }
    else {
        m_resource_barriers.push_back(barrier);
    }
}

void ResourceStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES state_after, UINT sub_resource) {
    if (resource) {
        D3D12_RESOURCE_BARRIER rb = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, state_after, sub_resource);
        ResourceBarrier(rb);
    }
}

void ResourceStateTracker::TransitionResource(const Resource& resource, D3D12_RESOURCE_STATES state_after, UINT sub_resource) {
    TransitionResource(resource.GetD3D12Resource().Get(), state_after, sub_resource);
}

void ResourceStateTracker::UAVBarrier(const Resource* resource) {
    ID3D12Resource* pResource = resource != nullptr ? resource->GetD3D12Resource().Get() : nullptr;
    ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(pResource));
}

void ResourceStateTracker::AliasBarrier(const Resource* resource_before, const Resource* resource_after) {
    ID3D12Resource* pResourceBefore = resource_before != nullptr ? resource_before->GetD3D12Resource().Get() : nullptr;
    ID3D12Resource* pResourceAfter = resource_after != nullptr ? resource_after->GetD3D12Resource().Get() : nullptr;
    ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(pResourceBefore, pResourceAfter));
}

void ResourceStateTracker::FlushResourceBarriers(const std::shared_ptr<CommandList>& command_list) {
    assert(command_list);

    UINT num_barriers = static_cast<UINT>(m_resource_barriers.size());
    if (num_barriers > 0u) {
        auto d3d12_command_list = command_list->GetD3D12CommandList();
        d3d12_command_list->ResourceBarrier(num_barriers, m_resource_barriers.data());
        m_resource_barriers.clear();
    }
}

uint32_t ResourceStateTracker::FlushPendingResourceBarriers(const std::shared_ptr<CommandList>& command_list) {
    assert(ms_is_locked);
    assert(command_list);

    ResourceBarriers resource_barriers;
    resource_barriers.reserve(m_pending_resource_barriers.size());

    for (auto pending_barrier : m_pending_resource_barriers) {
        if (pending_barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
            auto pending_transition = pending_barrier.Transition;
            const auto& iter = ms_global_resource_state.find(pending_transition.pResource);
            if (iter != ms_global_resource_state.end()) {
                auto& resource_state = iter->second;
                if (pending_transition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resource_state.SubresourceState.empty()) {
                    for (auto subresource_state : resource_state.SubresourceState) {
                        if (pending_transition.StateAfter != subresource_state.second) {
                            D3D12_RESOURCE_BARRIER new_barrier = pending_barrier;
                            new_barrier.Transition.Subresource = subresource_state.first;
                            new_barrier.Transition.StateBefore = subresource_state.second;
                            resource_barriers.push_back(new_barrier);
                        }
                    }
                }
                else {
                    auto global_state = (iter->second).GetSubresourceState(pending_transition.Subresource);
                    if (pending_transition.StateAfter != global_state) {
                        pending_barrier.Transition.StateBefore = global_state;
                        resource_barriers.push_back(pending_barrier);
                    }
                }
            }
        }
    }

    UINT num_barriers = static_cast<UINT>(resource_barriers.size());
    if (num_barriers > 0u) {
        auto d3d12CommandList = command_list->GetD3D12CommandList();
        d3d12CommandList->ResourceBarrier(num_barriers, resource_barriers.data());
    }

    m_pending_resource_barriers.clear();

    return num_barriers;
}

void ResourceStateTracker::CommitFinalResourceStates() {
    assert(ms_is_locked);

    for (const auto& resource_state : m_final_resource_state) {
        ms_global_resource_state[resource_state.first] = resource_state.second;
    }

    m_final_resource_state.clear();
}

void ResourceStateTracker::Reset() {
    m_pending_resource_barriers.clear();
    m_resource_barriers.clear();
    m_final_resource_state.clear();
}

void ResourceStateTracker::Lock() {
    ms_global_mutex.lock();
    ms_is_locked = true;
}

void ResourceStateTracker::Unlock() {
    ms_global_mutex.unlock();
    ms_is_locked = false;
}

void ResourceStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) {
    if (resource != nullptr) {
        std::lock_guard<std::mutex> lock(ms_global_mutex);
        ms_global_resource_state[resource].SetSubresourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
    }
}