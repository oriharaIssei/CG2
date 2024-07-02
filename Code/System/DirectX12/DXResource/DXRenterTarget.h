#pragma once

#include <vector>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class DXSwapChain;
class DXRenterTarget {
public:
	void Init(ID3D12Device *device,const DXSwapChain *swapChain);
	void Finalize();
private:
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTarget_;
public:
	ID3D12Resource *getRenderTarget(UINT index)const { return renderTarget_[index].Get(); }
};