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

	void CreateRootSignature(DirectXCommon* dxCommon);
	void InitInputLayout(D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc);
	void InitBlendState(D3D12_BLEND_DESC& blendDesc);
	void InitRasterizerState(D3D12_RASTERIZER_DESC& rasterizerDesc);

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> pipelineState_ = nullptr;
};