#pragma once

#include <Windows.h>
#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <cassert>
#include <cstdint>
#include <vector>

class WinApp;

class DirectXCommon {
public:
	DirectXCommon(WinApp *winApp):window_(winApp), barrierTransition_ { D3D12_RESOURCE_STATE_PRESENT } {};
	~DirectXCommon();

	void Init();
	void Finalize();

	void PreDraw();
	void PostDraw();

	void ClearRenderTarget();

	void ExecuteCommandList();
	void Wait4ExecuteCommand();
	void ResetCommand();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap *heap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap *heap, uint32_t descriptorSize, uint32_t index);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Create(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible
	);

	void CreateGraphicsPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc, ID3D12PipelineState *pipelineState);

	void CreateBufferResource(ID3D12Resource *resource, size_t sizeInBytes);
	void CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Resource> &resource, size_t sizeInBytes);

	void CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> &resource, int32_t width, int32_t height);

	class ResourceBarrierTransition {
	public:
		ResourceBarrierTransition(D3D12_RESOURCE_STATES firstState) { currentState_ = firstState; }
		D3D12_RESOURCE_BARRIER operator()(D3D12_RESOURCE_STATES stateAfter, ID3D12Resource *barrierTarget);
	private:
		D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_PRESENT;
	};
private:
	WinApp *window_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	//commandListにはcommandAllocatorが必要
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtv_ = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> swapChainResources_ = { nullptr };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsv_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = { nullptr };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srv_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvH_[2];

	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	ResourceBarrierTransition barrierTransition_;

	uint64_t fenceVal_ = 0;
private:
	void InitDXGIDevice();
	void InitCommand();
	void CreateSwapChain();
	void CreateRenderTarget();
	void CreateFence();
	void CreatDepthBuffer();
public:
	ID3D12Device *getDevice() { return device_.Get(); }
	IDXGISwapChain4 *getSwapChain() { return swapChain_.Get(); }
	ID3D12GraphicsCommandList *getCommandList() { return commandList_.Get(); }
	ID3D12CommandQueue *getCommandQueue() { return commandQueue_.Get(); }

	int getSwapChainBufferCount()const { return static_cast<int>(swapChainResources_.size()); }

	ID3D12DescriptorHeap *getRtv() { return rtv_.Get(); }
	ID3D12DescriptorHeap *getSrv() { return srv_.Get(); }
};