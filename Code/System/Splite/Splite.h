#pragma once

#include "wrl.h"
#include "d3d12.h"

class DirectXCommon;
class Splite {
public:
	void Init(DirectXCommon* dxCommon);
	void Finalize();

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> matrixBuff_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
};