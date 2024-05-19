#include "Material.h"

#include<System.h>

void Material::Init() {
	DirectXCommon *dxCommon = System::getInstance()->getDxCommon();
	dxCommon->CreateBufferResource(constBuff_, sizeof(ConstBufferMaterial));

	constBuff_->Map(
		0, nullptr, reinterpret_cast<void **>(&mappingData_)
	);
	materialData.color = { 1.0f,1.0f,1.0f,1.0f };
	materialData.enableLighting = 0;
}

void Material::ConvertToBuffer() {
	mappingData_->color = materialData.color;
	mappingData_->enableLighting = materialData.enableLighting;
	mappingData_->uvTransform = materialData.uvTransform;
}

void Material::Update() {
	UpdateMatrix();
	ConvertToBuffer();
}

void Material::UpdateMatrix() {
	materialData.uvTransform = MakeMatrix::Affine(uvTransformData);
}

void Material::SetForRootParameter(ID3D12GraphicsCommandList *cmdList, UINT rootParameterNum) const {
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum, constBuff_->GetGPUVirtualAddress());
}
