#include "DXSrvArray.h"

#include "System.h"
#include "TextureManager.h"

#include <assert.h>

void DXSrvArray::Finalize(){
	for(auto &resource : resources_){
		if(resource != nullptr){
			resource.Reset();
		}
	}
}

uint32_t DXSrvArray::CreateView(ID3D12Device *device,D3D12_SHADER_RESOURCE_VIEW_DESC &viewDesc,Microsoft::WRL::ComPtr<ID3D12Resource> resource){
	DXHeap *dxHeap = DXHeap::getInstance();

	uint32_t index = 0;
	for(; index < resources_.size(); index++){
		if(resources_[index] != nullptr){
			continue;
		}
		break;
	}
	device->CreateShaderResourceView(resource.Get(),&viewDesc,dxHeap->getSrvCpuHandle(arrayStartLocation_ + index));
	resources_[index] = resource;
	
	return index;
}

void DXSrvArray::DestroyView(uint32_t srvIndex){
	resources_[srvIndex].Reset();
	resources_[srvIndex] = nullptr;
}

void DXSrvArray::Init(uint32_t size,uint32_t arrayLocation){
	size_ = size;
	arrayStartLocation_ = arrayLocation;
	resources_.resize(size_);
}