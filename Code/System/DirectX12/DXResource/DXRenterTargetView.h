#pragma once

#include <array>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include "DXHeap.h"

#include "Vector2.h"
#include <stdint.h>

class DXSwapChain;
class DXRenterTargetView{
public:
	void Init(ID3D12Device *device,const DXSwapChain *swapChain);

	uint32_t CreateRenderTarget(uint32_t index,
								ID3D12Device *device,
								D3D12_RENDER_TARGET_VIEW_DESC desc,
								const Vector2 &windowSize,
								D3D12_HEAP_PROPERTIES heapProperties = D3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
								DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT,
								UINT16 arraySize = 1,
								UINT16 mipLevels = 0,
								UINT sampleCount = 1,
								UINT sampleQuality = 0,
								D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
								D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
								UINT64 alignment = 0);

	uint32_t CreateRenderTarget(ID3D12Device *device,
								D3D12_RENDER_TARGET_VIEW_DESC desc,
								const Vector2 &windowSize,
								D3D12_HEAP_PROPERTIES heapProperties = D3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
								DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT,
								UINT16 arraySize = 1,
								UINT16 mipLevels = 0,
								UINT sampleCount = 1,
								UINT sampleQuality = 0,
								D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
								D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
								UINT64 alignment = 0);
	void Finalize();
private:
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,DXHeap::rtvHeapSize> rtvArray_;
public:
	ID3D12Resource *getRenderTarget(UINT index)const{ return rtvArray_[index].Get(); }
};

using DXRtv = DXRenterTargetView;