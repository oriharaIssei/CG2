#pragma once

#include <Windows.h>
#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <cassert>
#include <cstdint>

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
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };
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
public:
	const Microsoft::WRL::ComPtr<ID3D12Device> getDevice()const { return device_.Get(); }
};