#include "Model.h"

#include <cassert>

#include "ModelManager.h"
#include "TextureManager.h"
#include <System.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <stdint.h>

std::unique_ptr<Matrix4x4> Model::fovMa_ = nullptr;

Model *Model::Create(const std::string &directoryPath, const std::string &filename) {
	static ModelManager modelManager;
	Model *model = new Model();
	modelManager.Create(model, directoryPath, filename);
	return model;
}

void Model::Init() {
	fovMa_ = std::make_unique<Matrix4x4>();
	Matrix4x4 *maPtr = new Matrix4x4();
	*maPtr = MakeMatrix::PerspectiveFov(
		0.45f,
		static_cast<float>(System::getInstance()->getWinApp()->getWidth())
		/
		static_cast<float>(System::getInstance()->getWinApp()->getHeight()),
		0.1f,
		100.0f);
	fovMa_.reset(
		maPtr
	);
}

void Model::Draw(const Matrix4x4 &world, const Matrix4x4 &view) {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();

	*buffer_->matrixData = { {
			world *
			view *
		*fovMa_.get()},
		{world}
	};

	dxCommon_->getCommandList()->SetGraphicsRootSignature(usePso_->rootSignature.Get());
	dxCommon_->getCommandList()->SetPipelineState(usePso_->pipelineState.Get());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &buffer_->vbView);
	//形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		buffer_->materialBuff->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, //RootParameter配列の 1 番目
		buffer_->matrixBuff->GetGPUVirtualAddress()
	);
	

	if(isTexture_ == true) {
		ID3D12DescriptorHeap *ppHeaps[] = { dxCommon_->getSrv() };
		dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
		dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
			3,
			TextureManager::getGpuHandle(*data_.materialData.textureNumber.get())
		);
	}

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced((UINT)(data_.vertSize), 1, 0, 0);
}

void Model::setUv(const Matrix4x4 &uv) {
	buffer_->materialData->uvTransform = uv;
}