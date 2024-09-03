#include "DXRtvArray.h"

void DXRtvArray::Finalize(){
	for(auto &resource : resources_){
		if(resource != nullptr){
			resource.Reset();
		}
	}
}

uint32_t DXRtvArray::CreateView(ID3D12Device *device,D3D12_RENDER_TARGET_VIEW_DESC &viewDesc,const Microsoft::WRL::ComPtr<ID3D12Resource> &resource){
	DXHeap *dxHeap = DXHeap::getInstance();

	uint32_t index = 0;
	for(; index < resources_.size(); index++){
		if(resources_[index] != nullptr){
			continue;
		}
		break;
	}
	device->CreateRenderTargetView(resource.Get(),&viewDesc,dxHeap->getRtvCpuHandle(arrayStartLocation_ + index));
	resources_[index] = resource;

	return index;
}

void DXRtvArray::DestroyView(uint32_t index){
	resources_[index].Reset();
	resources_[index] = nullptr;
}

void DXRtvArray::Init(uint32_t size,uint32_t arrayLocation){
	size_ = size;
	arrayStartLocation_ = arrayLocation;
	resources_.resize(size_);
}