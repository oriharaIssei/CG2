#pragma once

#include <Windows.h>
#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

/*
<TODO>
・画面の色を変えるコードを正しい位置に書く(正しい位置がわからないため保留中) - Init()
*/
class WinApp;
class DirectXCommon {
public:
	DirectXCommon(WinApp* winApp):window_(winApp) {};
	~DirectXCommon();

	void Init();

	void ClearRenderTarget();

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
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	
	ID3D12Debug1* debugController = nullptr;
private:
	void InitDXGIDevice();
	void InitCommand();
	void CreateSwapChain();
	void CreateRenderTarget();

public:
};