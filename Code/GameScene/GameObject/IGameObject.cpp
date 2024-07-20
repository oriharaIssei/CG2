#include "IGameObject.h"

#include <imgui.h>

#include "System.h"

void IGameObject::Init(const std::string &objectName){
	name_ = objectName;
	materialManager_ = System::getInstance()->getMaterialManager();
}

void IGameObject::Updata(){
	ImGui::Text("Name: %s",name_.c_str());
	ImGui::DragFloat3("Scale",&transform_->scale.x,0.01f);
	ImGui::DragFloat3("Rotate",&transform_->rotate.x,0.01f);
	ImGui::DragFloat3("Translate",&transform_->translate.x,0.01f);
	transform_->Update();
}