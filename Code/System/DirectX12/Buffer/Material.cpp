#include "Material.h"

#include <System.h>
#include "DXFunctionHelper.h"

void Material::Init() {
	DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),constBuff_,sizeof(ConstBufferMaterial));

	constBuff_->Map(
		0,nullptr,reinterpret_cast<void **>(&mappingData_)
	);

	mappingData_->color = {1.0f,1.0f,1.0f,1.0f};
	mappingData_->enableLighting = 0;
	mappingData_->uvTransform = MakeMatrix::Identity();
}

void Material::Finalize() {
	constBuff_.Reset();
}

void Material::SetForRootParameter(ID3D12GraphicsCommandList *cmdList,UINT rootParameterNum) const {
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum,constBuff_->GetGPUVirtualAddress());
}

std::shared_ptr<Material> MaterialManager::Create(const std::string &materialName) {
	if(materialPallete_.count(materialName) == 0) {
		materialPallete_[materialName] = std::make_shared<Material>();
		materialPallete_[materialName]->Init();
	}
	return materialPallete_[materialName];
}

std::shared_ptr<Material> MaterialManager::Create(const std::string &materialName,const MaterialData &data) {
	materialPallete_[materialName] = std::make_shared<Material>();
	materialPallete_[materialName]->Init();
	*materialPallete_[materialName]->mappingData_ = {
		.color = data.color,
		.enableLighting = data.enableLighting,
		.uvTransform = data.uvTransform
	};

	return materialPallete_[materialName];
}

void MaterialManager::Finalize() {
	for(auto &material : materialPallete_) {
		material.second->Finalize();
	}
}

void MaterialManager::Edit(const std::string &materialName,const MaterialData &data) {
	*materialPallete_[materialName]->mappingData_ = {
		.color = data.color,
		.enableLighting = data.enableLighting,
		.uvTransform = data.uvTransform
	};
}

void MaterialManager::EditColor(const std::string &materialName,const Vector4 &color) {
	materialPallete_[materialName]->mappingData_->color = color;
}

void MaterialManager::EditUvTransform(const std::string &materialName,const Transform &transform) {
	materialPallete_[materialName]->mappingData_->uvTransform = MakeMatrix::Affine(transform);
}

void MaterialManager::EditEnableLighting(const std::string &materialName,bool enableLighting) {
	materialPallete_[materialName]->mappingData_->enableLighting = enableLighting;
}