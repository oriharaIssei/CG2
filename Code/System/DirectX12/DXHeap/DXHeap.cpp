#include "DXHeap.h"

#include <cassert>

DXHeap *DXHeap::getInstance() {
	static DXHeap instance = DXHeap();
	return &instance;
}

void DXHeap::Init(ID3D12Device *device) {
	///================================================
	///	Heap の生成
	///================================================
	rtvHeap_ = CreateHeap(device,D3D12_DESCRIPTOR_HEAP_TYPE_RTV,3,false);
	srvHeap_ = CreateHeap(device,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,128,true);
	dsvHeap_ = CreateHeap(device,D3D12_DESCRIPTOR_HEAP_TYPE_DSV,1,false);

	rtvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	srvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DXHeap::Finalize() {
	rtvHeap_.Reset();
	srvHeap_.Reset();
	dsvHeap_.Reset();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DXHeap::CreateHeap(ID3D12Device *device,D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescriptors,bool shaderVisible) {
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc {};

	rtvDesc.Type = heapType;
	rtvDesc.NumDescriptors = numDescriptors;
	rtvDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device->CreateDescriptorHeap(
		&rtvDesc,
		IID_PPV_ARGS(&descriptorHeap)
	);
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}
