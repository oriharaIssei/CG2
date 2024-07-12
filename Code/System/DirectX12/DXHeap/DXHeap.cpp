#include "DXHeap.h"

#include <vector>

#include <cassert>

DXHeap *DXHeap::getInstance(){
	static DXHeap instance = DXHeap();
	return &instance;
}

void DXHeap::Init(ID3D12Device *device){
	///================================================
	///	Heap の生成
	///================================================
	rtvHeap_ = CreateHeap(device,D3D12_DESCRIPTOR_HEAP_TYPE_RTV,rtvHeapSize,false);
	srvHeap_ = CreateHeap(device,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,srvHeapSize,true);
	dsvHeap_ = CreateHeap(device,D3D12_DESCRIPTOR_HEAP_TYPE_DSV,dsvHeapSize,false);

	rtvIncrementSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvIncrementSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	srvIncrementSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DXHeap::CompactRtvHeap(ID3D12Device *device,std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE,uint32_t>> &usedDescriptorsArrays){
	D3D12_CPU_DESCRIPTOR_HANDLE dstHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();

	for(size_t i = 0; i < usedDescriptorsArrays.size(); ++i){
		if(usedDescriptorsArrays[i].first.ptr != dstHandle.ptr){
			device->CopyDescriptorsSimple(usedDescriptorsArrays[i].second,dstHandle,usedDescriptorsArrays[i].first,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		dstHandle.ptr += srvIncrementSize_ * usedDescriptorsArrays[i].second;
	}
}

void DXHeap::CompactSrvHeap(ID3D12Device *device,std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE,uint32_t>> &usedDescriptorsArrays){
	D3D12_CPU_DESCRIPTOR_HANDLE dstHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();

	for(size_t i = 0; i < usedDescriptorsArrays.size(); ++i){
		if(usedDescriptorsArrays[i].first.ptr != dstHandle.ptr){
			device->CopyDescriptorsSimple(usedDescriptorsArrays[i].second,dstHandle,usedDescriptorsArrays[i].first,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		dstHandle.ptr += srvIncrementSize_ * usedDescriptorsArrays[i].second;
	}
}

void DXHeap::CompactDsvHeap(ID3D12Device *device,std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE,uint32_t>> &usedDescriptorsArrays){
	D3D12_CPU_DESCRIPTOR_HANDLE dstHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();

	for(size_t i = 0; i < usedDescriptorsArrays.size(); ++i){
		if(usedDescriptorsArrays[i].first.ptr != dstHandle.ptr){
			device->CopyDescriptorsSimple(usedDescriptorsArrays[i].second,dstHandle,usedDescriptorsArrays[i].first,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		dstHandle.ptr += srvIncrementSize_ * usedDescriptorsArrays[i].second;
	}
}

void DXHeap::Finalize(){
	rtvHeap_.Reset();
	srvHeap_.Reset();
	dsvHeap_.Reset();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DXHeap::CreateHeap(ID3D12Device *device,D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescriptors,bool shaderVisible){
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};

	rtvDesc.Type = heapType;
	rtvDesc.NumDescriptors = numDescriptors;
	rtvDesc.Flags = shaderVisible?D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE:D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device->CreateDescriptorHeap(
		&rtvDesc,
		IID_PPV_ARGS(&descriptorHeap)
	);
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}
