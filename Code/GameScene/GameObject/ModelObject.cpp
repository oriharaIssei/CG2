#include "ModelObject.h"

#include <imgui.h>

#include "TextureManager.h"

#include "System.h"

void ModelObject::Init(const std::string &directryPath,const std::string &objectName){
	name_ = objectName;
	materialManager_ = System::getInstance()->getMaterialManager();

	model_ = Model::Create(directryPath,objectName + ".obj");

	transform_.Init();
}

void ModelObject::Updata(){
#ifdef _DEBUG
	ImGui::Begin(name_.c_str());
	IGameObject::Updata();

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
	model_->Draw(transform_,viewProj);
}