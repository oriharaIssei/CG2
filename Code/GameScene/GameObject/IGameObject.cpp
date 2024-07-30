#include "IGameObject.h"

#include <imgui.h>

#include "System.h"

std::list<std::pair<std::string,std::string>> IGameObject::textureList_;

void IGameObject::Init([[maybe_unused]]const std::string &directryPath,const std::string &objectName){
	name_ = objectName;
	materialManager_ = System::getInstance()->getMaterialManager();
}

void IGameObject::Updata(){
	ImGui::DragFloat3("Scale",&transform_.scale.x,0.1f);
	ImGui::DragFloat3("Rotate",&transform_.rotate.x,0.1f);
	ImGui::DragFloat3("Translate",&transform_.translate.x,0.1f);
	transform_.Update();

	ImGui::InputText("New MaterialName",materialName,32);
	if(ImGui::Button("Changing to new Material")){
		material_ = materialManager_->getMaterial(materialName);
	}
}