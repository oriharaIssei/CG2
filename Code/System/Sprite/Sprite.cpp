#include "Sprite.h"

#include "DirectXCommon.h"
#include "System.h"
#include "TextureManager.h"
#include <Logger.h>
#include <ShaderCompiler.h>

std::unique_ptr<PipelineStateObj> Sprite::pso_ = nullptr;
Matrix4x4 Sprite::viewPortMat_;

void Sprite::Init() {
	pso_ = std::make_unique<PipelineStateObj>();
	CreatePSO();
	WinApp *window = System::getInstance()->getWinApp();
	viewPortMat_ = MakeMatrix::Orthographic(0, 0, (float)window->getWidth(), (float)window->getHeight(), 0.0f, 100.0f);
}

void Sprite::Finalize() {
	pso_->Finalize();
	pso_.release();
}

Sprite *Sprite::Create(const Vector2 &pos, const Vector2 &size, const std::string &textureFilePath) {
	Sprite *result = new Sprite();
	result->th_ = TextureManager::LoadTexture(textureFilePath);

	result->meshBuff_ = std::make_unique<SpriteMesh>();
	result->meshBuff_->Init();

	result->meshBuff_->vertexData[0] = { {0.0f, size.y, 0.0f, 1.0f}, {0.0f, 1.0f} };
	result->meshBuff_->vertexData[1] = { {0, 0, 0.0f, 1.0f}, {0.0f, 0.0f} };
	result->meshBuff_->vertexData[2] = { { size.x,  size.y, 0.0f, 1.0f}, {1.0f, 1.0f} };
	result->meshBuff_->vertexData[3] = { {size.x, 0, 0.0f, 1.0f}, {1.0f, 0.0f} };

	result->mappingConstBufferData_ = nullptr;
	System::getInstance()->getDxCommon()->CreateBufferResource(result->constBuff_, sizeof(SpritConstBuffer));

	result->constBuff_->Map(
		0, nullptr, reinterpret_cast<void **>(&result->mappingConstBufferData_)
	);
	result->mappingConstBufferData_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	result->worldMat_ = MakeMatrix::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { pos.x,pos.y,1.0f });

	return result;
}

void Sprite::PreDraw() {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();

	dxCommon_->getCommandList()->SetGraphicsRootSignature(pso_->rootSignature.Get());
	dxCommon_->getCommandList()->SetPipelineState(pso_->pipelineState.Get());
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
}

void Sprite::CreatePSO() {
	DirectXCommon *dxCommon = System::getInstance()->getDxCommon();

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	// SRV を扱うように設定
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// offset を自動計算するように 設定
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
	D3D12_BLEND_DESC blendDesc {};
	D3D12_RASTERIZER_DESC rasterizerDesc {};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature {};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	///================================================
	/// Sampler の設定
	///================================================
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイナリニアフィルタ
	// 0 ~ 1 の間をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MinLOD = 0;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// DescriptorTable を使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	// パラメーターをDESCにセット
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);
	if(FAILED(hr)) {
		Logger::OutputLog(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリをもとに作成
	dxCommon->getDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&pso_->rootSignature)
	);
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";/*Semantics*/
	inputElementDescs[0].SemanticIndex = 0;/*Semanticsの横に書いてある数字(今回はPOSITION0なので 0 )*/
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float 4
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";/*Semantics*/
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	ShaderCompiler compiler;
	compiler.Init();

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Sprite.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Sprite.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc {};
	graphicsPipelineStateDesc.pRootSignature = pso_->rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize()
	};

	// DepthStancilState の設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc {};
	// Depth の機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual (近ければ描画する)
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencil の設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込む用のRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)タイプ。三角形を設定する
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 生成
	hr = dxCommon->getDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pso_->pipelineState)
	);
	assert(SUCCEEDED(hr));
}

void Sprite::Draw() {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();

	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &meshBuff_->vbView);
	dxCommon_->getCommandList()->IASetIndexBuffer(&meshBuff_->ibView);

	mappingConstBufferData_->mat_ = worldMat_ * viewPortMat_;

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, constBuff_->GetGPUVirtualAddress()
	);

	ID3D12DescriptorHeap *ppHeaps[] = { dxCommon_->getSrv() };
	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
		1,
		TextureManager::getDescriptorGpuHandle(th_)
	);
	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(1, TextureManager::getDescriptorGpuHandle(th_));
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	dxCommon_->getCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::SpriteMesh::Init() {
	DirectXCommon *dxCommon = System::getInstance()->getDxCommon();

	const size_t vertexBufferSize = sizeof(SpriteVertexData) * 6;
	const size_t indexBufferSize = sizeof(uint32_t) * 6;

	// バッファのリソースを作成
	dxCommon->CreateBufferResource(vertBuff, vertexBufferSize);
	dxCommon->CreateBufferResource(indexBuff, indexBufferSize);

	// 頂点バッファビューの設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertexBufferSize;
	vbView.StrideInBytes = sizeof(SpriteVertexData);

	// 頂点バッファをマップ
	vertBuff->Map(0, nullptr, reinterpret_cast<void **>(&vertexData));

	// インデックスバッファビューの設定
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = indexBufferSize;
	ibView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスバッファをマップ
	indexBuff->Map(0, nullptr, reinterpret_cast<void **>(&indexData));

	// インデックスデータの設定
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
}