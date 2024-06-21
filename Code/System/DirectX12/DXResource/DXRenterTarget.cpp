#include "DXRenterTarget.h"

#include "DXDevice.h"
#include "DXHeap.h"
#include "DXSwapChain.h"

#include <cassert>

void DXRenterTarget::Init(ID3D12Device *device,const DXSwapChain *swapChain) {
	UINT bufferCount = swapChain->getBufferCount();
	renderTarget_.resize(bufferCount);

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	DXHeap *heap = DXHeap::getInstance();

	// 各バックバッファのRTVを作成
	for(UINT i = 0; i < bufferCount; ++i) {
		renderTarget_[i] = swapChain->getBackBuffer(i);
		device->CreateRenderTargetView(renderTarget_[i].Get(),&rtvDesc,heap->getRtvHandle(i));
	}
}

void DXRenterTarget::Finalize() {
	for(auto &rt : renderTarget_) {
		rt.Reset();
	}
	renderTarget_.clear();
}
