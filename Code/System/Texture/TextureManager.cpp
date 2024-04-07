#include "TextureManager.h"

#include <combaseapi.h>
#include <cassert>

#include "ShaderCompiler.h"
#include "Logger.h"
#include "DirectXCommon.h"

DirectXCommon* TextureManager::dxCommon_ = nullptr;
std::vector<std::unique_ptr<TextureManager::Texture>> TextureManager::textures_;
std::unique_ptr<PipelineStateObj> TextureManager::pso_ = nullptr;
#pragma region Texture
void TextureManager::Texture::Init(const std::string& filePath,int textureIndex) {
	//==================================================
	// Textureを読み込んで転送する
	//==================================================
	DirectX::ScratchImage mipImages = Load(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	CreateTextureResource(dxCommon_, metadata);
	UploadTextureData(mipImages);

	//==================================================
	// ShaderResourceView を作成
	//==================================================
	/// metadataを基に SRV の設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	/// SRV を作成する DescriptorHeap の場所を決める
	/// 先頭は ImGui が使用しているので その次を使う
	srvHandleCPU_ = dxCommon_->GetCPUDescriptorHandle(
		dxCommon_->getSrvDescriptorHeap(),
		dxCommon_->getDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
		textureIndex);

	srvHandleGPU_ = dxCommon_->GetGPUDescriptorHandle(
		dxCommon_->getSrvDescriptorHeap(),
		dxCommon_->getDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
		textureIndex);

	/// SRV の作成
	dxCommon_->getDevice()->CreateShaderResourceView(
		resource_.Get(),
		&srvDesc,
		srvHandleCPU_
	);
}

void TextureManager::Texture::Finalize() {
	resource_.Reset();
}

DirectX::ScratchImage TextureManager::Texture::Load(const std::string& filePath) {
	DirectX::ScratchImage image{};

	// テクスチャファイルを読み込んでプログラムで使えるようにする
	std::wstring filePathW = Logger::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image
	);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImages
	);
	return mipImages;
}

void TextureManager::Texture::CreateTextureResource(DirectXCommon* dxCommon, const DirectX::TexMetadata& metadata) {
	//================================================
	// 1. metadata を基に Resource を設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);// mipMap の数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行 or Texture[]の配列数
	resourceDesc.Format = metadata.format; //texture の Format
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント 1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//================================================
	// 2. 利用する Heap の設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//================================================
	// 3. Resource の作成
	HRESULT hr;
	hr = dxCommon->getDevice()->CreateCommittedResource(
		&heapProperties,// heap の設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,// Clear最適値
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr));
}

void TextureManager::Texture::UploadTextureData(DirectX::ScratchImage& mipImg) {
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(
		dxCommon_->getDevice(),
		mipImg.GetImages(),
		mipImg.GetImageCount(),
		mipImg.GetMetadata(),
		subResources
	);

	uint64_t intermediateSize = GetRequiredIntermediateSize(
		resource_.Get(),
		0,
		UINT(subResources.size())
	);

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;
	dxCommon_->CreateBufferResource(
		intermediateResource,
		intermediateSize
	);

	UpdateSubresources(
		dxCommon_->getCommandList(),
		resource_.Get(),
		intermediateResource.Get(),
		0,
		0,
		UINT(subResources.size()),
		subResources.data()
	);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCommon_->getCommandList()->ResourceBarrier(1, &barrier);

	ID3D12GraphicsCommandList* commandList = dxCommon_->getCommandList();

	commandList->Close();
	dxCommon_->ExecuteCommandList();
	dxCommon_->Wait4ExecuteCommand();
	dxCommon_->ResetCommand();

}
#pragma endregion

void TextureManager::Init(DirectXCommon* dxCommon) {
	CoInitializeEx(0, COINIT_MULTITHREADED);
	dxCommon_ = dxCommon;
	pso_ = std::make_unique<PipelineStateObj>();
	InitPSO();
}

void TextureManager::InitPSO() {
	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	// SRV を扱うように設定
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// offset を自動計算するように 設定
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

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

	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	// CBV を使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// PixelShderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// レジスタ番号0 とバインド
	// register(b0) の 0. b11 なら 11
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// VertexShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	// DescriptorTable を使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;
	
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
		Logger::OutputLog(reinterpret_cast<char*>( errorBlob->GetBufferPointer() ));
		assert(false);
	}

	//バイナリをもとに作成
	dxCommon_->getDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&pso_->rootSignature)
	);
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";/*Semantics*/
	inputElementDescs[0].SemanticIndex = 0;/*Semanticsの横に書いてある数字(今回はPOSITION0なので 0 )*/
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float 4
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";/*Semantics*/
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";/*Semantics*/
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Object3d.PS.hlsl", L"ps_6_0");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
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
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
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
	hr = dxCommon_->getDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pso_->pipelineState)
	);
	assert(SUCCEEDED(hr));
};

void TextureManager::SetPSO2CommandList(ID3D12GraphicsCommandList* cmdlist) {
	cmdlist->SetGraphicsRootSignature(pso_->rootSignature.Get());
	cmdlist->SetPipelineState(pso_->pipelineState.Get());
}

void TextureManager::Finalize() {
	CoUninitialize();
	for(auto& texture : textures_) {
		texture->Finalize();
	}
	pso_->Finalize();
	pso_.reset();
}

int TextureManager::LoadTexture(const std::string& filePath) {
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->Init(filePath, textures_.size());
	return static_cast<int>( textures_.size() - 1 );
}