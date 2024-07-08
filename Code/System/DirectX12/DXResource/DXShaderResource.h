#pragma once

#include <d3d12.h>
#include <externals/DirectXTex/DirectXTex.h>

#include <wrl.h>

#include <array>

#include <DXHeap.h>

class TextureManager;
class DXShaderResource {
	friend class TextureManager;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureSize">テクスチャーマネージャーで静的に確保されている数</param>
	void Finalize();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateResource(
		ID3D12Device *device,
		D3D12_RESOURCE_DESC resourceDesc,
		D3D12_HEAP_PROPERTIES heapProperties,
		D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON,
		const D3D12_CLEAR_VALUE *clearValue = nullptr
	);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device *device,const DirectX::TexMetadata &metadata,uint32_t index);
private:
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,DXHeap::srvHeapSize> resources_;
};