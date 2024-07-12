#pragma once

#include <wrl.h>

#include "WinApp.h"

#include "DXCommand.h"
#include "DXDepthStencilView.h"
#include "DXDevice.h"
#include "DXFence.h"
#include "DXHeap.h"
#include "DXRenterTargetView.h"
#include "DXSwapChain.h"

#include "ResourceBarrierManager.h"

class DXFunctionHelper {
public:
	static void CreateBufferResource(const DXDevice *dxDevice,Microsoft::WRL::ComPtr<ID3D12Resource> &resource,size_t sizeInBytes);

	static void ClearRenderTarget(const DXCommand *dxCommand,const DXSwapChain *dxSwapChain);

	static void SetViewportsAndScissor(const DXCommand *dxCommand,const WinApp *window);
	static void SetRenderTargets(const DXCommand *dxCommand,const DXSwapChain *dxSwapChain);
	static void PreDraw(const DXCommand *dxCommand,const WinApp* window,const DXSwapChain *dxSwapChain);
	static void PostDraw(DXCommand *dxCommands,DXFence *dxFence,DXSwapChain *dxSwapChain);
};
using DXFH = DXFunctionHelper;