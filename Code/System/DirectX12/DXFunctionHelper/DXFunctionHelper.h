#pragma once

#include <wrl.h>

#include "WinApp.h"

#include "DXCommand.h"
#include "DXDevice.h"
#include "DXFence.h"
#include "DXHeap.h"
#include "DXSwapChain.h"
#include "ResourceBarrierManager.h"

#include "Vector4.h"

class DXFunctionHelper{
public:
	static void CreateBufferResource(const DXDevice* dxDevice,Microsoft::WRL::ComPtr<ID3D12Resource>& resource,size_t sizeInBytes);
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device,uint32_t width,uint32_t height,DXGI_FORMAT format,const Vector4& clearColor);

	static void ClearRenderTarget(const DXCommand* dxCommand,const DXSwapChain* dxSwapChain);

	static void SetViewportsAndScissor(const DXCommand* dxCommand,const WinApp* window);
	static void SetRenderTargets(const DXCommand* dxCommand,const DXSwapChain* dxSwapChain);
	static void PreDraw(const DXCommand* dxCommand,const WinApp* window,const DXSwapChain* dxSwapChain);
	static void PostDraw(DXCommand* dxCommands,DXFence* dxFence,DXSwapChain* dxSwapChain);
};
using DXFH = DXFunctionHelper;