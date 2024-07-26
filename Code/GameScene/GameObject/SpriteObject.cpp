#include "SpriteObject.h"

#include <imgui.h>

#include "System.h"

void SpriteObject::Init(const std::string &directory,const std::string &objectName){
	name_ = objectName;
	materialManager_ = System::getInstance()->getMaterialManager();

	sprite_.reset(Sprite::Create({0.0f,0.0f},{20.0f,20.0f},directory + '/' + objectName + ".png"));
}

void SpriteObject::Updata(){
	ImGui::Text("Name: %s",name_.c_str());
	ImGui::DragFloat2("Position",&pos_.x,0.1f);
	ImGui::DragFloat2("Size",&size_.x,0.1f);
}

void SpriteObject::Draw(const ViewProjection &viewProj){
	viewProj;
	sprite_->Draw();
}