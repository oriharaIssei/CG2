#pragma once

#include <wrl.h>

#include <d3d12.h>


///=================================================
/// Pipeline State Object
///=================================================
class DirectXCommon;
class PipelineStateObj {
public:
	void Init(DirectXCommon* dxCommon);
private:
	void CreateRootSignature(DirectXCommon* dxCommon);
	void InitInputLayout(D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc);
	void InitBlendState(D3D12_BLEND_DESC& blendDesc);
	void InitRasterizerState(D3D12_RASTERIZER_DESC& rasterizerDesc);
private:
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> pipelineState_ = nullptr;
};

