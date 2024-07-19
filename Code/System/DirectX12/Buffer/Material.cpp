#include "Material.h"

#include "DXFunctionHelper.h"
#include <System.h>

#include <imgui.h>

void Material::Init(){
	DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),constBuff_,sizeof(ConstBufferMaterial));

	constBuff_->Map(
		0,nullptr,reinterpret_cast<void **>(&mappingData_)
	);

	mappingData_->color = {1.0f,1.0f,1.0f,1.0f};
	mappingData_->enableLighting = 0;
	mappingData_->uvTransform = MakeMatrix::Identity();
}

void Material::Finalize(){
	constBuff_.Reset();
}

void Material::SetForRootParameter(ID3D12GraphicsCommandList *cmdList,UINT rootParameterNum) const{
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum,constBuff_->GetGPUVirtualAddress());
}

Material *MaterialManager::Create(const std::string &materialName){
	if(materialPallete_.count(materialName) == 0){
		materialPallete_[materialName] = std::make_unique<Material>();
		materialPallete_[materialName]->Init();
	}
	return materialPallete_[materialName].get();
}

Material *MaterialManager::Create(const std::string &materialName,const MaterialData &data){
	materialPallete_[materialName] = std::make_unique<Material>();
	materialPallete_[materialName]->Init();
	*materialPallete_[materialName]->mappingData_ = {
		.color = data.color,
		.enableLighting = data.enableLighting,
		.uvTransform = data.uvTransform
	};

	return materialPallete_[materialName].get();
}

void MaterialManager::DebugUpdate(){
#ifdef _DEBUG
	ImGui::Begin("Materials");
	bool isEnableLighting = false;
	for(auto &material : materialPallete_){
		if(ImGui::TreeNode(material.first.c_str())){
			ImGui::TreePop();
			ImGui::ColorEdit4(std::string(material.first + "Color").c_str(),&material.second->mappingData_->color.x);
			isEnableLighting = static_cast<bool>(material.second->mappingData_->enableLighting);
			ImGui::Checkbox(std::string(material.first + "is EnableLighting").c_str(),&isEnableLighting);
			material.second->mappingData_->enableLighting = static_cast<uint32_t>(isEnableLighting);
		}
	}
	ImGui::End();
#endif // _DEBUG
}

void MaterialManager::Finalize(){
	for(auto &material : materialPallete_){
		material.second->Finalize();
	}
}

void MaterialManager::Edit(const std::string &materialName,const MaterialData &data){
	*materialPallete_[materialName]->mappingData_ = {
		.color = data.color,
		.enableLighting = data.enableLighting,
		.uvTransform = data.uvTransform
	};
}

void MaterialManager::EditColor(const std::string &materialName,const Vector4 &color){
	materialPallete_[materialName]->mappingData_->color = color;
}

void MaterialManager::EditUvTransform(const std::string &materialName,const Transform &transform){
	materialPallete_[materialName]->mappingData_->uvTransform = MakeMatrix::Affine(transform);
}

void MaterialManager::EditEnableLighting(const std::string &materialName,bool enableLighting){
	materialPallete_[materialName]->mappingData_->enableLighting = enableLighting;
}

void MaterialManager::DeleteMaterial(const std::string &materialName){
	materialPallete_[materialName].reset();
	materialPallete_[materialName] = nullptr;

	std::erase_if(materialPallete_,[](const auto &pair){
		return pair.second == nullptr;
	});
}