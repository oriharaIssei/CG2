#include "ModelObject.h"

#include <imgui.h>

void ModelObject::Init(const std::string &objectName){
	IGameObject::Init(objectName);
	transform_ = std::make_unique<WorldTransform>();
	transform_->Init();
}

void ModelObject::Updata(){
#ifdef _DEBUG
	ImGui::Text("Name: %s",name_.c_str());

	ImGui::DragFloat3("Scale",&transform_->scale.x,0.1f);
	ImGui::DragFloat3("Rotate",&transform_->rotate.x,0.1f);
	ImGui::DragFloat3("Translate",&transform_->translate.x,0.1f);
	transform_->Update();


#endif // _DEBUG
}

void ModelObject::Draw(const ViewProjection &viewProj){
	model_->Draw(*transform_.get(),viewProj,material_);
}