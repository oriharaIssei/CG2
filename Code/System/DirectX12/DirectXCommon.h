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
	DirectXCommon(WinApp* winApp) :window_(winApp) {};
	~DirectXCommon();

	void Init();
	void Finalize();

	void PreDraw();
	void PostDraw();

	void ClearRenderTarget();

	void ExecuteCommandList();
	void Wait4ExecuteCommand();
	void ResetCommand();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible
	);

	void CreateGraphicsPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, ID3D12PipelineState* pipelineState);

	void CreateBufferResource(ID3D12Resource* resource, size_t sizeInBytes);
	void CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, size_t sizeInBytes);

	void CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource>& resource,int32_t width, int32_t height);

	static void CheckIsAliveInstance();
private:
	WinApp* window_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	//commandListにはcommandAllocatorが必要
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> swapChainResources_ = { nullptr };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = { nullptr };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvH_[2];

	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	uint64_t fenceVal_ = 0;
private:
	void InitDXGIDevice();
	void InitCommand();
	void CreateSwapChain();
	void CreateRenderTarget();
	void CreateFence();
	void CreatDepthBuffer();
public:
	ID3D12Device* getDevice(){ return device_.Get(); }
	IDXGISwapChain4* getSwapChain() { return swapChain_.Get(); }
	ID3D12GraphicsCommandList* getCommandList() { return commandList_.Get(); }
	ID3D12CommandQueue* getCommandQueue() { return commandQueue_.Get(); }
	
	int getSwapChainBufferCount()const { return static_cast<int>(swapChainResources_.size()); }

	ID3D12DescriptorHeap* getRtvDescriptorHeap() { return rtvDescriptorHeap_.Get(); }
	ID3D12DescriptorHeap* getSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }
};