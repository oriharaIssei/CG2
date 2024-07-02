#pragma once

#include <wrl.h>

#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

class DXDevice;
class DXCommand;
class WinApp;
class DXSwapChain {
public:
	void Init(const WinApp *winApp,const DXDevice *device,const DXCommand *command);
	void Finalize();

	void Present();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	UINT bufferCount_;
public:
	UINT getBufferCount()const { return bufferCount_; }
	UINT getCurrentBackBufferIndex()const { return swapChain_->GetCurrentBackBufferIndex(); }

	IDXGISwapChain4 *getSwapChain()const { return swapChain_.Get(); }

	ID3D12Resource *getBackBuffer(UINT index)const { return backBuffers_[index].Get(); }
	ID3D12Resource *getCurrentBackBuffer()const { return backBuffers_[swapChain_->GetCurrentBackBufferIndex()].Get(); }
};