#include "ViewProjection.h"

#include <System.h>

void ViewProjection::Init() {
	DirectXCommon *dxCommon = System::getInstance()->getDxCommon();
	dxCommon->CreateBufferResource(buff_, sizeof(ConstBufferDataViewProjection));
	buff_->Map(0, nullptr, reinterpret_cast<void **>(&mappingData_));
}

void ViewProjection::UpdateMatrix() {
	viewMat = MakeMatrix::Affine({ 1.0f,1.0f,1.0f }, rotate, translate);
	projectionMat = MakeMatrix::PerspectiveFov(fovAngleY, aspectRatio, nearZ, farZ);
}

void ViewProjection::ConvertToBuffer() {
	mappingData_->view = viewMat;
	mappingData_->projection = projectionMat;
}

void ViewProjection::SetForRootParameter(ID3D12GraphicsCommandList *cmdList, UINT rootParameterNum)const {
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum, buff_->GetGPUVirtualAddress());
}