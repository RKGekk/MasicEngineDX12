#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

class CommandList;
class Resource;

class ResourceStateTracker {
public:
	ResourceStateTracker();
	virtual ~ResourceStateTracker();

	void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

	void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES state_after, UINT sub_resource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	void TransitionResource(const Resource& resource, D3D12_RESOURCE_STATES state_after, UINT sub_resource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

	void UAVBarrier(const Resource* resource = nullptr);
	void AliasBarrier(const Resource* resource_before = nullptr, const Resource* resource_after = nullptr);

	uint32_t FlushPendingResourceBarriers(const std::shared_ptr<CommandList>& command_list);
	void FlushResourceBarriers(const std::shared_ptr<CommandList>& command_list);

	void CommitFinalResourceStates();

	void Reset();

	static void Lock();
	static void Unlock();

	static void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);

protected:
private:
	using ResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;

	ResourceBarriers m_pending_resource_barriers;
	ResourceBarriers m_resource_barriers;

	struct ResourceState {
		explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON) : State(state) {}

		void SetSubresourceState(UINT subresource, D3D12_RESOURCE_STATES state) {
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
				State = state;
				SubresourceState.clear();
			}
			else {
				SubresourceState[subresource] = state;
			}
		}

		D3D12_RESOURCE_STATES GetSubresourceState(UINT subresource) const {
			D3D12_RESOURCE_STATES state = State;
			const auto iter = SubresourceState.find(subresource);
			if (iter != SubresourceState.end()) {
				state = iter->second;
			}
			return state;
		}

		D3D12_RESOURCE_STATES State;
		std::map<UINT, D3D12_RESOURCE_STATES> SubresourceState;
	};

	using ResourceList = std::vector<ID3D12Resource*>;
	using ResourceStateMap = std::unordered_map<ID3D12Resource*, ResourceState>;

	ResourceStateMap m_final_resource_state;

	static ResourceStateMap ms_global_resource_state;

	static std::mutex ms_global_mutex;
	static bool ms_is_locked;
};