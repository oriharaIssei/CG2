#include "System.h"

#include "DXHeap.h"
#include "DXFunctionHelper.h"

#include <imgui.h>
#include <ImGuiManager.h>
#include <PrimitiveDrawer.h>
#include <Sprite.h>
#include <TextureManager.h>

#include "DXRtvArrayManager.h"
#include "DXSrvArrayManager.h"

#include <Logger.h>

#define _USE_MATH_DEFINES
#include <cmath>

//.hに書いてはいけない
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")

System* System::getInstance(){
	static System instance;
	return &instance;
}

void System::Init(){
	window_ = std::make_unique<WinApp>();
	window_->CreateGameWindow(L"title",WS_OVERLAPPEDWINDOW,1280,720);

	input_ = Input::getInstance();
	input_->Init();

	dxDevice_ = std::make_unique<DXDevice>();
	dxDevice_->Init();

	DXHeap* dxHeap = DXHeap::getInstance();
	dxHeap->Init(dxDevice_->getDevice());

	dxCommand_ = std::make_unique<DXCommand>();
	dxCommand_->Init(dxDevice_->getDevice(),"main","main");

	dxSwapChain_ = std::make_unique<DXSwapChain>();
	dxSwapChain_->Init(window_.get(),dxDevice_.get(),dxCommand_.get());

	dxDsv_ = std::make_unique<DXDsv>();
	dxDsv_->Init(dxDevice_->getDevice(),dxHeap->getDsvHeap(),window_->getWidth(),window_->getHeight());

	DXSrvArrayManager::getInstance()->Init();

	DXRtvArrayManager::getInstance()->Init();

	dxFence_ = std::make_unique<DXFence>();
	dxFence_->Init(dxDevice_->getDevice());

	ShaderManager::getInstance()->Init();

	texturePsoKeys_ = {
		"Tex_Blend_None",
		"Tex_Blend_Normal",
		"Tex_Blend_Add",
		"Tex_Blend_Sub",
		"Tex_Blend_Multiply",
		"Tex_Blend_Screen"
	};

	CreateTexturePSO();

	ImGuiManager::getInstance()->Init(window_.get(),dxDevice_.get(),dxSwapChain_.get());

	TextureManager::Init();

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Init();
	directionalLight_->ConvertToBuffer();

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Init();
	pointLight_->ConvertToBuffer();

	spotLight_ = std::make_unique<SpotLight>();
	spotLight_->Init();
	spotLight_->ConvertToBuffer();

	PrimitiveDrawer::Init();
	Model::Init();
	Sprite::Init();

	materialManager_ = std::make_unique<MaterialManager>();
}

void System::Finalize(){
	directionalLight_->Finalize();
	pointLight_->Finalize();
	spotLight_->Finalize();
	materialManager_->Finalize();

	ShaderManager::getInstance()->Finalize();
	PrimitiveDrawer::Finalize();
	Sprite::Finalize();
	Model::Finalize();
	TextureManager::Finalize();

	DXSrvArrayManager::getInstance()->Finalize();
	DXRtvArrayManager::getInstance()->Finalize();
	dxDsv_->Finalize();

	DXHeap::getInstance()->Finalize();
	dxSwapChain_->Finalize();
	dxCommand_->Finalize();
	DXCommand::ResetAll();
	dxFence_->Finalize();
	dxDevice_->Finalize();

	input_->Finalize();

#ifdef _DEBUG
	ImGuiManager::getInstance()->Finalize();
#endif // _DEBUG
}

void System::CreateTexturePSO(){
	ShaderManager* shaderManager_ = ShaderManager::getInstance();
	///=================================================
	/// shader読み込み
	///=================================================
	shaderManager_->LoadShader("Object3dTexture.VS");
	shaderManager_->LoadShader("Object3dTexture.PS",shaderDirectory,L"ps_6_0");

	///=================================================
	/// shader情報の設定
	///=================================================
	ShaderInfo texShaderInfo{};
	texShaderInfo.vsKey = "Object3dTexture.VS";
	texShaderInfo.psKey = "Object3dTexture.PS";

#pragma region"RootParameter"
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter.Descriptor.ShaderRegister = 0;
	texShaderInfo.pushBackRootParameter(rootParameter);

	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter.Descriptor.ShaderRegister = 2;
	texShaderInfo.pushBackRootParameter(rootParameter);

	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter.Descriptor.ShaderRegister = 0;
	texShaderInfo.pushBackRootParameter(rootParameter);

	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter.Descriptor.ShaderRegister = 1;
	texShaderInfo.pushBackRootParameter(rootParameter);

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	// SRV を扱うように設定
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// offset を自動計算するように 設定
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// DescriptorTable を使う
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	size_t rootParameterIndex = texShaderInfo.pushBackRootParameter(rootParameter);
	texShaderInfo.SetDescriptorRange2Parameter(descriptorRange,1,rootParameterIndex);
#pragma endregion

	///=================================================
	/// Sampler
	D3D12_STATIC_SAMPLER_DESC staticSampler = {};
	staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイナリニアフィルタ
	// 0 ~ 1 の間をリピート
	staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSampler.MinLOD = 0;
	staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
	staticSampler.ShaderRegister = 0;
	staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	texShaderInfo.pushBackSamplerDesc(staticSampler);
	/// Sampler
	///=================================================

#pragma region "InputElement"
	D3D12_INPUT_ELEMENT_DESC inputElementDesc = {};
	inputElementDesc.SemanticName = "POSITION";/*Semantics*/
	inputElementDesc.SemanticIndex = 0;/*Semanticsの横に書いてある数字(今回はPOSITION0なので 0 )*/
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float 4
	inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	texShaderInfo.pushBackInputElementDesc(inputElementDesc);

	inputElementDesc.SemanticName = "TEXCOORD";/*Semantics*/
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	texShaderInfo.pushBackInputElementDesc(inputElementDesc);

	inputElementDesc.SemanticName = "NORMAL";/*Semantics*/
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	texShaderInfo.pushBackInputElementDesc(inputElementDesc);
#pragma endregion

	///=================================================
	/// BlendMode ごとの Psoを作成
	///=================================================
	for(size_t i = 0; i < kBlendNum; ++i){
		texShaderInfo.blendMode_ = static_cast<BlendMode>(i);
		shaderManager_->CreatePso(texturePsoKeys_[i],texShaderInfo,dxDevice_->getDevice());
	}
}

bool System::ProcessMessage(){
	return window_->ProcessMessage();
}

void System::BeginFrame(){
	ImGuiManager::getInstance()->Begin();
	input_->Update();
	PrimitiveDrawer::ResetInstanceVal();

	PrimitiveDrawer::setBlendMode(BlendMode::Alpha);
	Sprite::setBlendMode(BlendMode::Alpha);
}

void System::EndFrame(){
	ImGuiManager::getInstance()->End();
	ImGuiManager::getInstance()->Draw();
}

void System::ScreenPreDraw(){
	DXFH::PreDraw(dxCommand_.get(),window_.get(),dxSwapChain_.get());
}

void System::ScreenPostDraw(){
	DXFH::PostDraw(dxCommand_.get(),dxFence_.get(),dxSwapChain_.get());
}

int System::LoadTexture(const std::string& filePath){
	return TextureManager::LoadTexture(filePath);
}