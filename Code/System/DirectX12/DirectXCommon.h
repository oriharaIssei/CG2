#pragma once
#include "DirectXInclude.cpp"

class DirectXCommon {
public:
	DirectXCommon() {};
	~DirectXCommon();

	bool Init();
private:
	IDXGIFactory7* dxgiFactory_ = nullptr;
	IDXGIAdapter4* useAdapter_ = nullptr;
	ID3D12Device* device_ = nullptr;
};

