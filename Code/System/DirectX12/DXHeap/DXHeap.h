#pragma once

#include <wrl.h>

#include <d3d12.h>

class DXHeap {
public:
	static DXHeap *getInstance();
public:
	void Init(ID3D12Device *device);
	void Finalize();
private:
	DXHeap() = default;
	~DXHeap() = default;
	DXHeap(const DXHeap &) = delete;
	DXHeap &operator=(const DXHeap &) = delete;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device *device,D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescriptors,bool shaderVisible);
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;

	UINT rtvDescriptorSize_;
	UINT dsvDescriptorSize_;
	UINT srvDescriptorSize_;
public:
	ID3D12DescriptorHeap *getRtvHeap()const { return rtvHeap_.Get(); }
	ID3D12DescriptorHeap *getSrvHeap()const { return srvHeap_.Get(); }
	ID3D12DescriptorHeap *getDsvHeap()const { return dsvHeap_.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE getRtvHandle(UINT index)const { return D3D12_CPU_DESCRIPTOR_HANDLE(index * rtvDescriptorSize_ + rtvHeap_->GetCPUDescriptorHandleForHeapStart().ptr); }
	D3D12_CPU_DESCRIPTOR_HANDLE getDsvHandle(UINT index)const { return D3D12_CPU_DESCRIPTOR_HANDLE(index * dsvDescriptorSize_ + dsvHeap_->GetCPUDescriptorHandleForHeapStart().ptr); }
};