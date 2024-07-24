#include "ModelObject.h"

#include <imgui.h>

#include "TextureManager.h"

#include "System.h"

void ModelObject::Init(const std::string &objectName){
	IGameObject::Init(objectName);
	model_ = Model::Create(defaultReosurceFolder,objectName + ".obj");
	transform_ = std::make_unique<WorldTransform>();
	transform_->Init();
}

void ModelObject::Updata(){
#ifdef _DEBUG
	ImGui::Begin(name_.c_str());

	ImGui::DragFloat3("Scale",&transform_->scale.x,0.1f);
	ImGui::DragFloat3("Rotate",&transform_->rotate.x,0.1f);
	ImGui::DragFloat3("Translate",&transform_->translate.x,0.1f);
	transform_->Update();

	for(auto &data : model_->getData()){
		if(ImGui::BeginChild("TextureList")){
			for(auto &texture : textureList_){
				ImGui::Bullet();
				if(!ImGui::Button(texture.second.c_str())){
					continue;
				}
				*data->materialData.textureNumber = TextureManager::LoadTexture(texture.first + texture.second);
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
#endif // _DEBUG
}

void ModelObject::Draw(const ViewProjection &viewProj){
	model_->Draw(*transform_.get(),viewProj,material_);
}