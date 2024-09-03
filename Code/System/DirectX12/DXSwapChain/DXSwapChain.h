#pragma once

#include <memory>
#include <wrl.h>

#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "DXRtvArray.h"

class DXDevice;
class DXCommand;
class WinApp;
class DXSwapChain{
public:
	void Init(const WinApp *winApp,const DXDevice *device,const DXCommand *command);
	void Finalize();

	void Present();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::shared_ptr<DXRtvArray> backBuffers_;
	UINT bufferCount_;
public:
	UINT getBufferCount()const{ return bufferCount_; }
	UINT getCurrentBackBufferIndex()const{ return swapChain_->GetCurrentBackBufferIndex(); }

	IDXGISwapChain4 *getSwapChain()const{ return swapChain_.Get(); }

	ID3D12Resource *getBackBuffer(UINT index)const{ return backBuffers_->getRtv(index); }
	ID3D12Resource *getCurrentBackBuffer()const{ return backBuffers_->getRtv(swapChain_->GetCurrentBackBufferIndex()); }
};