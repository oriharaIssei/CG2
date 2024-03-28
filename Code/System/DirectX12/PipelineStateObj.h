#pragma once

#include <wrl.h>

#include <d3d12.h>

///=================================================
/// Pipeline State Object
///=================================================
class DirectXCommon;
struct PipelineStateObj {
	void Init(DirectXCommon* dxCommon);
	void Finalize();

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> pipelineState_ = nullptr;
};