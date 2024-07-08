#include "DXShaderResource.h"

#include <assert.h>

void DXShaderResource::Finalize() {
	for(auto &resource : resources_) {
		if(resource != nullptr) {
			resource.Reset();
		}
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> DXShaderResource::CreateResource(ID3D12Device *device,
																		D3D12_RESOURCE_DESC resourceDesc,
																		D3D12_HEAP_PROPERTIES heapProperties,
																		D3D12_HEAP_FLAGS flag,
																		D3D12_RESOURCE_STATES resourceState,
																		const D3D12_CLEAR_VALUE *clearValue) {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT resutl = device->CreateCommittedResource(
		&heapProperties,
		flag,
		&resourceDesc,
		resourceState,
		clearValue,// Clear最適値
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(resutl));
	return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DXShaderResource::CreateTextureResource(ID3D12Device *device,const DirectX::TexMetadata &metadata,uint32_t index) {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	//================================================
	// 1. metadata を基に Resource を設定
	D3D12_RESOURCE_DESC resourceDesc {};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);// mipMap の数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行 or Texture[]の配列数
	resourceDesc.Format = metadata.format; //texture の Format
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント 1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//================================================
	// 2. 利用する Heap の設定
	D3D12_HEAP_PROPERTIES heapProperties {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//================================================
	// 3. Resource の作成
	HRESULT hr;
	hr = device->CreateCommittedResource(
		&heapProperties,// heap の設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,// Clear最適値
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	resources_[index] = resource;
	return resource;
}