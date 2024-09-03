#include "System.h"

#include "DXFunctionHelper.h"

#include <imgui.h>
#include <ImGuiManager.h>
#include <PrimitiveDrawer.h>
#include <Sprite.h>
#include <TextureManager.h>

#include "DXSrvArrayManager.h"

#include <Logger.h>

#define _USE_MATH_DEFINES
#include <cmath>

//.hに書いてはいけない
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")

System *System::getInstance(){
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

	DXHeap::getInstance()->Init(dxDevice_->getDevice());

	dxCommand_ = std::make_unique<DXCommand>();
	dxCommand_->Init(dxDevice_->getDevice(),"main","main");

	dxSwapChain_ = std::make_unique<DXSwapChain>();
	dxSwapChain_->Init(window_.get(),dxDevice_.get(),dxCommand_.get());

	dxRenderTarget_ = std::make_unique<DXRenterTargetView>();
	dxRenderTarget_->Init(dxDevice_->getDevice(),dxSwapChain_.get());

	DXSrvArrayManager::getInstance()->Init();

	dxDepthStencil_ = std::make_unique<DXDepthStencilView>();
	dxDepthStencil_->Init(dxDevice_->getDevice(),DXHeap::getInstance()->getDsvHeap(),window_->getWidth(),window_->getHeight());

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
	ShaderManager::getInstance()->Finalize();
	PrimitiveDrawer::Finalize();
	Sprite::Finalize();
	Model::Finalize();
	TextureManager::Finalize();
	DXSrvArrayManager::getInstance()->Finalize();

	dxDevice_->Finalize();
	DXHeap::getInstance()->Finalize();
	dxDepthStencil_->Finalize();
	dxRenderTarget_->Finalize();
	dxSwapChain_->Finalize();
	dxCommand_->Finalize();
	DXCommand::ResetAll();
	dxFence_->Finalize();

	input_->Finalize();

	materialManager_->Finalize();
	directionalLight_->Finalize();
	pointLight_->Finalize();
	spotLight_->Finalize();

#ifdef _DEBUG
	ImGuiManager::getInstance()->Finalize();
#endif // _DEBUG
}

//
//void System::DrawSphere(const Matrix4x4 &world, const Matrix4x4 &view, const Vector4 &color) {
//	constexpr int kSubDivision=16;
//	const float kLonEvery=static_cast<float>(M_PI) * 2.0f / static_cast<float>(kSubDivision);
//	const float kLatEvery=static_cast<float>(M_PI) / static_cast<float>(kSubDivision);
//
//	for(int latIndex=0; latIndex < kSubDivision; ++latIndex) {
//		float lat=-static_cast<float>(M_PI) / 2.0f + (kLatEvery * latIndex);
//		for(int lonIndex=0; lonIndex < kSubDivision; ++lonIndex) {
//			float lon=lonIndex * kLonEvery;
//
//			uint32_t startIndex=(latIndex * kSubDivision + lonIndex) * 6;
//
//			primitiveBuff_[Sphere]->indexData[startIndex]=startIndex;
//			primitiveBuff_[Sphere]->indexData[startIndex + 1]=startIndex + 1;
//			primitiveBuff_[Sphere]->indexData[startIndex + 2]=startIndex + 2;
//
//			primitiveBuff_[Sphere]->indexData[startIndex + 3]=startIndex + 1;
//			primitiveBuff_[Sphere]->indexData[startIndex + 4]=startIndex + 3;
//			primitiveBuff_[Sphere]->indexData[startIndex + 5]=startIndex + 2;
//
//			// lb 0,0
//			primitiveBuff_[Sphere]->vertData[startIndex].pos={ Vector4(
//				std::cosf(lat) * std::cosf(lon),
//				std::sinf(lat),
//				std::cosf(lat) * std::sinf(lon),
//				1.0f)
//			};
//			primitiveBuff_[Sphere]->vertData[startIndex].color=color;
//
//			// lt 0,1
//			primitiveBuff_[Sphere]->vertData[startIndex + 1].pos={ Vector4(
//				std::cosf(lat + kLatEvery) * std::cosf(lon),
//				std::sinf(lat + kLatEvery),
//				std::cosf(lat + kLatEvery) * std::sinf(lon),
//				1.0f)
//			};
//			primitiveBuff_[Sphere]->vertData[startIndex + 1].color=color;
//
//			//rb 1,0
//			primitiveBuff_[Sphere]->vertData[startIndex + 2].pos={ Vector4(
//				std::cosf(lat) * std::cosf(lon + kLonEvery),
//				std::sinf(lat),
//				std::cosf(lat) * std::sinf(lon + kLonEvery),
//				1.0f)
//			};
//			primitiveBuff_[Sphere]->vertData[startIndex + 2].color=color;
//
//			// rt 1,1
//			primitiveBuff_[Sphere]->vertData[startIndex + 3].pos={ Vector4(
//				std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
//				std::sinf(lat + kLatEvery),
//				std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),
//				1.0f)
//			};
//			primitiveBuff_[Sphere]->vertData[startIndex + 3].color=color;
//		}
//	}
//
//	Matrix4x4 *wvp3d=nullptr;
//	wvp3DResource_->Map(0, nullptr, reinterpret_cast<void **>(&wvp3d));
//	*wvp3d=world * view * MakeMatrix::PerspectiveFov(
//		0.45f,
//		static_cast<float>(window_->getWidth())
//		/
//		static_cast<float>(window_->getHeight()),
//		0.1f,
//		100.0f
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootSignature(primitivePSO_->rootSignature.Get());
//	dxCommon_->getCommandList()->SetPipelineState(primitivePSO_->pipelineState.Get());
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &primitiveBuff_[Sphere]->vbView);
//	dxCommon_->getCommandList()->IASetIndexBuffer(&primitiveBuff_[Sphere]->ibView);
//	//形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1, //RootParameter配列の 1 番目
//		wvp3DResource_->GetGPUVirtualAddress()
//	);
//
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawIndexedInstanced(primitiveBuff_[Sphere]->vertexNum, 1, 0, 0, 0);
//}

//void System::DrawTriangleTexture(const Vector3 &position1, const Vector3 &position2, const Vector3 &position3, const Vector3 &scale, const Matrix4x4 &wvp, Vector4 *color, int textureNum) {
//	VertexData *vertData=nullptr;
//
//	primitiveBuff_[Triangle]->vertBuff->Map(
//		0,
//		nullptr,
//		reinterpret_cast<void **>(&vertData)
//	);
//
//	vertData[0].pos={ -0.5f,-0.5f,0.0f,1.0f };
//	vertData[0].texCoord={ 0.0f,1.0f };
//
//	vertData[1].pos={ 0.0f,0.5f,0.0f,1.0f };
//	vertData[1].texCoord={ 0.5,0.0f };
//
//	vertData[2].pos={ 0.5f,-0.5f,0.0f,1.0f };
//	vertData[2].texCoord={ 1.0f,1.0f };
//
//	Matrix4x4 *wvpData=nullptr;
//	wvp2DResource_->Map(0, nullptr, reinterpret_cast<void **>(&wvpData));
//	*wvpData=wvp;
//
//	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &primitiveBuff_[Triangle]->vbView);
//	//形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		0, //RootParameter配列の 0 番目
//		spliteBuff_[Quad]->materialBuff->GetGPUVirtualAddress()
//	);
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1,
//		spliteBuff_[Quad]->matrixBuff->GetGPUVirtualAddress()
//	);
//
//	ID3D12DescriptorHeap *ppHeaps[]={ dxCommon_->getSrv() };
//	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
//
//	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
//		2,
//		TextureManager::getGpuHandle(textureNum)
//	);
//
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawInstanced(3, 1, 0, 0);
//}
//
//void System::DrawSprite(const Vector3 &ltPos, const Vector2 &size, const Vector3 &uvScale, const Vector3 &uvRotate, const Vector3 &uvTranslate, int textureNum) {
//	spliteBuff_[Quad]->vertData[0].pos={ ltPos.x,ltPos.y + size.y,ltPos.z,1.0f };
//	spliteBuff_[Quad]->vertData[0].texCoord={ 0.0f,1.0f };
//	spliteBuff_[Quad]->vertData[1].pos={ ltPos.x,ltPos.y,ltPos.z,1.0f };
//	spliteBuff_[Quad]->vertData[1].texCoord={ 0.0,0.0f };
//	spliteBuff_[Quad]->vertData[2].pos={ ltPos.x + size.x,ltPos.y + size.y,ltPos.z,1.0f };
//	spliteBuff_[Quad]->vertData[2].texCoord={ 1.0f,1.0f };
//	spliteBuff_[Quad]->vertData[3].pos={ ltPos.x + size.x,ltPos.y,ltPos.z,1.0f };
//	spliteBuff_[Quad]->vertData[3].texCoord={ 1.0f,0.0f };
//
//	spliteBuff_[Quad]->materialData->color={ 1.0f,1.0f,1.0f,1.0f };
//	spliteBuff_[Quad]->materialData->uvTransform=
//		MakeMatrix::Scale(uvScale)
//		* MakeMatrix::RotateZ(uvRotate.z)
//		* MakeMatrix::Translate(uvTranslate);
//
//	spliteBuff_[Quad]->matrixData->wvp=MakeMatrix::Orthographic(0.0f, 0.0f, static_cast<float>(window_->getWidth()), static_cast<float>(window_->getHeight()), 0.0f, 100.0f);
//
//	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
//	dxCommon_->getCommandList()->IASetIndexBuffer(&spliteBuff_[Quad]->ibView);`
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &spliteBuff_[Quad]->vbView);
//	// 形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		0, //RootParameter配列の 0 番目
//		spliteBuff_[Quad]->materialBuff->GetGPUVirtualAddress()
//	);
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1,
//		spliteBuff_[Quad]->matrixBuff->GetGPUVirtualAddress()
//	);
//
//	ID3D12DescriptorHeap *ppHeaps[]={ dxCommon_->getSrv() };
//	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
//
//	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
//		2,
//		TextureManager::getGpuHandle(textureNum)
//	);
//
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
//}
//
//void System::DrawModel(const Matrix4x4 &world, const Matrix4x4 &view, int textureNum) {
//	*spliteBuff_[Sphere]->matrixData={ {
//			world *
//			view *
//		MakeMatrix::PerspectiveFov(
//			0.45f,
//			static_cast<float>(window_->getWidth())
//			/
//			static_cast<float>(window_->getHeight()),
//			0.1f,
//			100.0f)},
//		{world}
//	};
//
//	ImGui::Begin("Light");
//	DirectionalLight light=*spliteBuff_[Sphere]->lightData;
//	ImGui::DragFloat3("Color", &light.color.x, 0.01f);
//	ImGui::DragFloat3("direction", &light.direction.x, 0.01f);
//	ImGui::DragFloat("Intensity", &light.intensity, 0.01f);
//	light.direction=light.direction.Normalize();
//	*spliteBuff_[Sphere]->lightData=light;
//	ImGui::End();
//
//	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &model_->vbv);
//	//形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		0, //RootParameter配列の 0 番目
//		spliteBuff_[Sphere]->materialBuff->GetGPUVirtualAddress()
//	);
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1, //RootParameter配列の 1 番目
//		spliteBuff_[Sphere]->matrixBuff->GetGPUVirtualAddress()
//	);
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		3, //RootParameter配列の 3 番目
//		spliteBuff_[Sphere]->lightBuff->GetGPUVirtualAddress()
//	);
//
//	ID3D12DescriptorHeap *ppHeaps[]={ dxCommon_->getSrv() };
//	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
//
//	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
//		2,
//		TextureManager::getGpuHandle(textureNum)
//	);
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawInstanced((UINT)(model_->data_.vertices.size()), 1, 0, 0);
//}
//
//void System::DrawSpherTexture(const Matrix4x4 &world, const Matrix4x4 &view, int textureNum) {
//	constexpr int kSubDivision=16;
//	const float kLonEvery=static_cast<float>(M_PI) * 2.0f / static_cast<float>(kSubDivision);
//	const float kLatEvery=static_cast<float>(M_PI) / static_cast<float>(kSubDivision);
//
//	for(int latIndex=0; latIndex < kSubDivision; ++latIndex) {
//		float lat=-static_cast<float>(M_PI) / 2.0f + (kLatEvery * latIndex);
//		for(int lonIndex=0; lonIndex < kSubDivision; ++lonIndex) {
//			float lon=lonIndex * kLonEvery;
//
//			uint32_t startIndex=(latIndex * kSubDivision + lonIndex) * 6;
//
//			spliteBuff_[Sphere]->indexData[startIndex]=startIndex;
//			spliteBuff_[Sphere]->indexData[startIndex + 1]=startIndex + 1;
//			spliteBuff_[Sphere]->indexData[startIndex + 2]=startIndex + 2;
//
//			spliteBuff_[Sphere]->indexData[startIndex + 3]=startIndex + 1;
//			spliteBuff_[Sphere]->indexData[startIndex + 4]=startIndex + 3;
//			spliteBuff_[Sphere]->indexData[startIndex + 5]=startIndex + 2;
//
//			// lb 0,0
//			spliteBuff_[Sphere]->vertData[startIndex].pos={ Vector4(
//				std::cosf(lat) * std::cosf(lon),
//				std::sinf(lat),
//				std::cosf(lat) * std::sinf(lon),
//				1.0f)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex].texCoord={
//				float(lonIndex) / float(kSubDivision),
//				1.0f - float(latIndex) / float(kSubDivision)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex].normal={
//				spliteBuff_[Sphere]->vertData[startIndex].pos.x,
//				spliteBuff_[Sphere]->vertData[startIndex].pos.y ,
//				spliteBuff_[Sphere]->vertData[startIndex].pos.z
//			};
//
//			// lt 0,1
//			spliteBuff_[Sphere]->vertData[startIndex + 1].pos={ Vector4(
//				std::cosf(lat + kLatEvery) * std::cosf(lon),
//				std::sinf(lat + kLatEvery),
//				std::cosf(lat + kLatEvery) * std::sinf(lon),
//				1.0f)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex + 1].texCoord={
//				float(lonIndex) / float(kSubDivision),
//				1.0f - float(latIndex + 1.0f) / float(kSubDivision)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex + 1].normal={
//				spliteBuff_[Sphere]->vertData[startIndex + 1].pos.x,
//				spliteBuff_[Sphere]->vertData[startIndex + 1].pos.y ,
//				spliteBuff_[Sphere]->vertData[startIndex + 1].pos.z
//			};
//
//			//rb 1,0
//			spliteBuff_[Sphere]->vertData[startIndex + 2].pos={ Vector4(
//				std::cosf(lat) * std::cosf(lon + kLonEvery),
//				std::sinf(lat),
//				std::cosf(lat) * std::sinf(lon + kLonEvery),
//				1.0f)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex + 2].texCoord={
//				float(lonIndex + 1.0f) / float(kSubDivision) ,
//				1.0f - float(latIndex) / float(kSubDivision)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex + 2].normal={
//				spliteBuff_[Sphere]->vertData[startIndex + 2].pos.x,
//				spliteBuff_[Sphere]->vertData[startIndex + 2].pos.y ,
//				spliteBuff_[Sphere]->vertData[startIndex + 2].pos.z
//			};
//			// rt 1,1
//			spliteBuff_[Sphere]->vertData[startIndex + 3].pos={ Vector4(
//				std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
//				std::sinf(lat + kLatEvery),
//				std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),
//				1.0f)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex + 3].texCoord={
//				float(lonIndex + 1.0f) / float(kSubDivision) ,
//				1.0f - float(latIndex + 1.0f) / float(kSubDivision)
//			};
//			spliteBuff_[Sphere]->vertData[startIndex + 3].normal={
//				spliteBuff_[Sphere]->vertData[startIndex + 3].pos.x,
//				spliteBuff_[Sphere]->vertData[startIndex + 3].pos.y ,
//				spliteBuff_[Sphere]->vertData[startIndex + 3].pos.z
//			};
//		}
//	}
//
//	*spliteBuff_[Sphere]->matrixData={ {
//			world *
//			view *
//		MakeMatrix::PerspectiveFov(
//			0.45f,
//			static_cast<float>(window_->getWidth())
//			/
//			static_cast<float>(window_->getHeight()),
//			0.1f,
//			100.0f)},
//		{world}
//	};
//
//	ImGui::Begin("Light");
//	DirectionalLight light=*spliteBuff_[Sphere]->lightData;
//	ImGui::DragFloat3("Color", &light.color.x, 0.01f);
//	ImGui::DragFloat3("direction", &light.direction.x, 0.01f);
//	ImGui::DragFloat("Intensity", &light.intensity, 0.01f);
//	light.direction=light.direction.Normalize();
//	*spliteBuff_[Sphere]->lightData=light;
//	ImGui::End();
//
//	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &spliteBuff_[Sphere]->vbView);
//	dxCommon_->getCommandList()->IASetIndexBuffer(&spliteBuff_[Sphere]->ibView);
//	//形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		0, //RootParameter配列の 0 番目
//		spliteBuff_[Sphere]->materialBuff->GetGPUVirtualAddress()
//	);
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1, //RootParameter配列の 1 番目
//		spliteBuff_[Sphere]->matrixBuff->GetGPUVirtualAddress()
//	);
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		3, //RootParameter配列の 1 番目
//		spliteBuff_[Sphere]->lightBuff->GetGPUVirtualAddress()
//	);
//
//	ID3D12DescriptorHeap *ppHeaps[]={ dxCommon_->getSrv() };
//	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
//
//	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
//		2,
//		TextureManager::getGpuHandle(textureNum)
//	);
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawIndexedInstanced((UINT)(kSubDivision * kSubDivision * 6), 1, 0, 0, 0);
//}

void System::CreateTexturePSO(){
	ShaderManager *shaderManager_ = ShaderManager::getInstance();
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
		texShaderInfo.blendMode_=static_cast<BlendMode>(i);
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

	DXFH::PreDraw(dxCommand_.get(),window_.get(),dxSwapChain_.get());
}

void System::EndFrame(){
	ImGuiManager::getInstance()->End();
	ImGuiManager::getInstance()->Draw();
	DXFH::PostDraw(dxCommand_.get(),dxFence_.get(),dxSwapChain_.get());
}

int System::LoadTexture(const std::string &filePath){
	return TextureManager::LoadTexture(filePath);
}