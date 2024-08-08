#include "ResourceBarrierManager.h"

std::unordered_map<ID3D12Resource *,D3D12_RESOURCE_STATES> ResourceBarrierManager::resourceStates_;

D3D12_RESOURCE_BARRIER ResourceBarrierManager::Barrier(ID3D12Resource *resource,D3D12_RESOURCE_STATES stateAfter) {
	auto it = resourceStates_.find(resource);
	if(it == resourceStates_.end()) {
		// 未登録のリソースには何もしないか、初期状態を設定する
		RegisterReosurce(resource,D3D12_RESOURCE_STATE_COMMON);
		it = resourceStates_.find(resource);
	}

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = it->second;
	barrier.Transition.StateAfter = stateAfter;

	// 状態を更新
	it->second = stateAfter;

	return barrier;
}
