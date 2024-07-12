#include "DXRenterTargetView.h"

#include "DXDevice.h"
#include "DXHeap.h"
#include "DXSwapChain.h"

#include "DXFunctionHelper.h"

#include <cassert>

void DXRenterTargetView::Init(ID3D12Device *device,const DXSwapChain *swapChain){
	UINT bufferCount = swapChain->getBufferCount();
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	DXHeap *heap = DXHeap::getInstance();

	// 各バックバッファのRTVを作成
	for(UINT i = 0; i < bufferCount; ++i){
		rtvArray_[i] = swapChain->getBackBuffer(i);
		device->CreateRenderTargetView(rtvArray_[i].Get(),&rtvDesc,heap->getRtvCpuHandle(i));
	}
}

uint32_t DXRenterTargetView::CreateRenderTarget(uint32_t index,
												ID3D12Device *device,
												D3D12_RENDER_TARGET_VIEW_DESC desc,
												const Vector2 &windowSize,
												D3D12_HEAP_PROPERTIES heapProperties,
												DXGI_FORMAT format,
												UINT16 arraySize,UINT16 mipLevels,UINT sampleCount,UINT sampleQuality,
												D3D12_RESOURCE_FLAGS flags,
												D3D12_TEXTURE_LAYOUT layout,
												UINT64 alignment){
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;

	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
	format,
	static_cast<UINT64>(windowSize.x),
	static_cast<UINT>(windowSize.y),
	arraySize,
	mipLevels,
	sampleCount,
	sampleQuality,
	flags,
	layout,
	alignment
	);

	device->CreateCommittedResource(
	&heapProperties,
	D3D12_HEAP_FLAG_NONE,
	&resourceDesc,
	D3D12_RESOURCE_STATE_RENDER_TARGET,
	nullptr,
	IID_PPV_ARGS(&resource)
	);

	device->CreateRenderTargetView(resource.Get(),&desc,DXHeap::getInstance()->getRtvCpuHandle(static_cast<UINT>(index)));
	rtvArray_[index] = resource;

	return index;
}

uint32_t DXRenterTargetView::CreateRenderTarget(ID3D12Device *device,D3D12_RENDER_TARGET_VIEW_DESC desc,const Vector2 &windowSize,D3D12_HEAP_PROPERTIES heapProperties,DXGI_FORMAT format,UINT16 arraySize,UINT16 mipLevels,UINT sampleCount,UINT sampleQuality,D3D12_RESOURCE_FLAGS flags,D3D12_TEXTURE_LAYOUT layout,UINT64 alignment){
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;

	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
	format,
	static_cast<UINT64>(windowSize.x),
	static_cast<UINT>(windowSize.y),
	arraySize,
	mipLevels,
	sampleCount,
	sampleQuality,
	flags,
	layout,
	alignment
	);

	device->CreateCommittedResource(
	&heapProperties,
	D3D12_HEAP_FLAG_NONE,
	&resourceDesc,
	D3D12_RESOURCE_STATE_RENDER_TARGET,
	nullptr,
	IID_PPV_ARGS(&resource)
	);

	uint32_t index = 0;
	for(; index < rtvArray_.size(); ++index){
		if(rtvArray_[index] != nullptr){
			continue;
		}
		break;
	}

	device->CreateRenderTargetView(resource.Get(),&desc,DXHeap::getInstance()->getRtvCpuHandle(static_cast<UINT>(index)));
	rtvArray_[index] = resource;

	return index;
}

void DXRenterTargetView::Finalize(){
	for(auto &rtv : rtvArray_){
		rtv.Reset();
	}
}