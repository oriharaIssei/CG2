#include "WorldTransform.h"

#include <System.h>

void WorldTransform::Init() {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();

	dxCommon_->CreateBufferResource(buff_, sizeof(ConstantBufferWorldMatrix));
	buff_->Map(0, nullptr, reinterpret_cast<void **>(&mappingWorldMat_));

	worldMat = MakeMatrix::Identity();
}

void WorldTransform::Update() {
	worldMat = MakeMatrix::Affine(transformData);

	if(parent != nullptr) {
		worldMat *= parent->worldMat;
	}
	ConvertToBuffer();
}

void WorldTransform::ConvertToBuffer() {
	mappingWorldMat_->world = worldMat;
}

void WorldTransform::SetForRootParameter(ID3D12GraphicsCommandList *cmdList, UINT rootParameterNum) const {
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum, buff_->GetGPUVirtualAddress());
}
