#include "Material.h"

#include "DXFunctionHelper.h"
#include <System.h>

#include <imgui.h>

const char *lightTypes[] = {"NONE","HALF_LAMBERT","LAMBERT"};

void Material::Init(){
	DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),constBuff_,sizeof(ConstBufferMaterial));

	constBuff_->Map(
		0,nullptr,reinterpret_cast<void **>(&mappingData_)
	);

	mappingData_->color = {1.0f,1.0f,1.0f,1.0f};
	mappingData_->enableLighting = 0;

	uvScale_ = {1.0f,1.0f,1.0f};
	uvRotate_ = {0.0f,0.0f,0.0f};
	uvTranslate_ = {0.0f,0.0f,0.0f};

	mappingData_->uvTransform = MakeMatrix::Affine(uvScale_,uvRotate_,uvTranslate_);
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
		if(!ImGui::TreeNode(material.first.c_str())){
			continue;
		}
		ImGui::TreePop();
		ImGui::ColorEdit4(std::string(material.first + "Color").c_str(),&material.second->mappingData_->color.x);
		isEnableLighting = static_cast<bool>(material.second->mappingData_->enableLighting);

		ImGui::Combo("Lighting Type",
					 (int *)&material.second->mappingData_->enableLighting,
					 lightTypes,
					 IM_ARRAYSIZE(lightTypes),
					 3);

		ImGui::DragFloat3("uvScale",&material.second->uvScale_.x,0.1f);
		ImGui::DragFloat3("uvRotate",&material.second->uvRotate_.x,0.1f);
		ImGui::DragFloat3("uvTranslate",&material.second->uvTranslate_.x,0.1f);

		material.second->mappingData_->uvTransform = MakeMatrix::Affine(material.second->uvScale_,material.second->uvRotate_,material.second->uvTranslate_);
	}

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::InputText("MaterialName",newMaterialName_,64);	
	if(ImGui::Button("Create",{64,24})){
		Create(newMaterialName_);
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