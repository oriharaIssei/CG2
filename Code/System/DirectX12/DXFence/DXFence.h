#pragma once

#include <wrl.h>

#include <d3dx12.h>

class DXFence {
public:
	void Init(ID3D12Device *device);
	void Finalize();
	void Signal(ID3D12CommandQueue *commandQueue) {
		commandQueue->Signal(fence_.Get(),++fenceValue_);
	};
	void WaitForFence();

private:
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_;
public:
	ID3D12Fence *getFence()const { return fence_.Get(); }
};