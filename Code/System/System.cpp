#include "System.h"

#include <CommonBuffer.h>
#include <imgui.h>
#include <ImGuiManager.h>
#include <Logger.h>
#include <TextureManager.h>

#define _USE_MATH_DEFINES
#include <cmath>

System *System::getInstance() {
	static System instance;
	return &instance;
}

void System::Init() {
	window_ = std::make_unique<WinApp>();
	window_->CreateGameWindow(L"title", WS_OVERLAPPEDWINDOW, 1280, 720);

	dxCommon_ = std::make_unique<DirectXCommon>(window_.get());
	dxCommon_->Init();

	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Init();

	primitivePso_=std::make_unique<PipelineStateObj>();
	texturePso_=std::make_unique<PipelineStateObj>();
	CreateTexturePSO();
	CreatePrimitivePSO();

	CommonBuffer::Init(dxCommon_.get());
	
	TextureManager::Init(dxCommon_.get());
	
	ImGuiManager::getInstance()->Init(window_.get(), dxCommon_.get());
}

void System::Finalize() {
	dxCommon_->Finalize();
	shaderCompiler_->Finalize();

	TextureManager::Finalize();
	CommonBuffer::Finalize();
#ifdef _DEBUG
	ImGuiManager::getInstance()->Finalize();
#endif // _DEBUG
}

//void System::DrawTriangle(const Vector3 &position1, const Vector3 &position2, const Vector3 &position3, const Vector3 &scale, Vector4 color) {
//	size_t indexVertex=primitiveBuff_[Triangle]->index * primitiveBuff_[Triangle]->vertexNum;
//
//	primitiveBuff_[Triangle]->vertData[indexVertex].pos={ position1.x,position1.y,position1.z,1.0f };
//	primitiveBuff_[Triangle]->vertData[indexVertex].color=color;
//	primitiveBuff_[Triangle]->vertData[indexVertex + 1].pos={ position2.x,position2.y,position2.z,1.0f };
//	primitiveBuff_[Triangle]->vertData[indexVertex + 1].color=color;
//	primitiveBuff_[Triangle]->vertData[indexVertex + 2].pos={ position3.x,position3.y,position3.z,1.0f };
//	primitiveBuff_[Triangle]->vertData[indexVertex + 2].color=color;
//
//	dxCommon_->getCommandList()->SetGraphicsRootSignature(primitivePSO_->rootSignature.Get());
//	dxCommon_->getCommandList()->SetPipelineState(primitivePSO_->pipelineState.Get());
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &primitiveBuff_[Triangle]->vbView);
//	//形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1, //RootParameter配列の 1 番目
//		wvp2DResource_->GetGPUVirtualAddress()
//	);
//
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawInstanced(primitiveBuff_[Triangle]->vertexNum, 1, indexVertex, 0);
//	++primitiveBuff_[Triangle]->index;
//}
//
//void System::DrawQuad(const Vector3 &lt, const Vector3 &rt, const Vector3 &lb, const Vector3 &rb, const Vector3 &scale, Vector4 color) {
//	size_t indexVertex=primitiveBuff_[Quad]->index * primitiveBuff_[Quad]->vertexNum;
//
//	primitiveBuff_[Quad]->vertData[indexVertex].pos={ lb.x,lb.y,lb.z,1.0f };
//	primitiveBuff_[Quad]->vertData[indexVertex].color=color;
//	primitiveBuff_[Quad]->vertData[indexVertex + 1].pos={ lt.x,lt.y,lt.z,1.0f };
//	primitiveBuff_[Quad]->vertData[indexVertex + 1].color=color;
//	primitiveBuff_[Quad]->vertData[indexVertex + 2].pos={ rb.x,rb.y,rb.z,1.0f };
//	primitiveBuff_[Quad]->vertData[indexVertex + 2].color=color;
//	primitiveBuff_[Quad]->vertData[indexVertex + 3].pos={ rt.x,rt.y,rt.z,1.0f };
//	primitiveBuff_[Quad]->vertData[indexVertex + 3].color=color;
//
//	dxCommon_->getCommandList()->SetGraphicsRootSignature(primitivePSO_->rootSignature.Get());
//	dxCommon_->getCommandList()->SetPipelineState(primitivePSO_->pipelineState.Get());
//	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &primitiveBuff_[Quad]->vbView);
//	dxCommon_->getCommandList()->IASetIndexBuffer(&primitiveBuff_[Quad]->ibView);
//	//形状設定.PSOのものとはまた別(同じものを設定する)
//	dxCommon_->getCommandList()->IASetPrimitiveTopology(
//		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
//	);
//
//	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
//		1, //RootParameter配列の 1 番目
//		wvp2DResource_->GetGPUVirtualAddress()
//	);
//
//	// 描画!!!
//	dxCommon_->getCommandList()->DrawIndexedInstanced(primitiveBuff_[Quad]->vertexNum, 1, 0, 0, 0);
//	++primitiveBuff_[Quad]->index;
//}
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
//	dxCommon_->getCommandList()->IASetIndexBuffer(&spliteBuff_[Quad]->ibView);
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

void System::CreatePrimitivePSO() {
	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
	D3D12_BLEND_DESC blendDesc {};
	D3D12_RASTERIZER_DESC rasterizerDesc {};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature {};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	// CBV を使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// PixelShderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// レジスタ番号0 とバインド
	// register(b0) の 0. b11 なら 11
	rootParameters[0].Descriptor.ShaderRegister = 3;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// VertexShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 5;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].Descriptor.ShaderRegister = 4;

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
	dxCommon_->getDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&primitivePso_->rootSignature)
	);
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";/*Semantics*/
	inputElementDescs[0].SemanticIndex = 0;/*Semanticsの横に書いてある数字(今回はPOSITION0なので 0 )*/
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float 4
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "NORMAL";/*Semantics*/
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
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

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Object3d.PS.hlsl", L"ps_6_0");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc {};
	graphicsPipelineStateDesc.pRootSignature = primitivePso_->rootSignature.Get();
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
	hr = dxCommon_->getDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&primitivePso_->pipelineState)
	);
	assert(SUCCEEDED(hr));
}

void System::CreateTexturePSO() {
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

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].Descriptor.ShaderRegister = 1;

	// DescriptorTable を使う
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

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
	dxCommon_->getDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&texturePso_->rootSignature)
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

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Object3dTexture.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Object3dTexture.PS.hlsl", L"ps_6_0");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc {};
	graphicsPipelineStateDesc.pRootSignature = texturePso_->rootSignature.Get();
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
	hr = dxCommon_->getDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&texturePso_->pipelineState)
	);
	assert(SUCCEEDED(hr));
}

bool System::ProcessMessage() {
	return window_->ProcessMessage();
}

void System::BeginFrame() {
	ImGuiManager::getInstance()->Begin();
	dxCommon_->PreDraw();
}

void System::EndFrame() {
	ImGuiManager::getInstance()->End();
	ImGuiManager::getInstance()->Draw(dxCommon_.get());
	dxCommon_->PostDraw();

}

int System::LoadTexture(const std::string &filePath) {
	return TextureManager::LoadTexture(filePath);
}