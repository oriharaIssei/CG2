#include "DebugCamera.h"

#include <imgui.h>

void DebugCamera::Init() {
	input_ = Input::getInstance();
	currentState_.reset(new Neutral(this));
}

void DebugCamera::DebugUpdate() {
	ImGui::Begin("DebugCamera");
	ImGui::SliderFloat3("Rotate",&viewProj_.rotate.x,-3,3);
	ImGui::SliderFloat3("Translate",&viewProj_.translate.x,-3,3);
	ImGui::End();
}

void DebugCamera::Update() {
	if(currentState_) {
		currentState_->Update();
	}
	viewProj_.UpdateMatrix();
}

void DebugCamera::Neutral::Update() {
	if(!(host_->input_->isPressKey(DIK_LALT) || host_->input_->isPressKey(DIK_RALT))) {
		return;
	}
	if(host_->input_->isTriggerMouseButton(0)) {
		host_->currentState_.reset(new TranslationState(host_));
		return;
	}
	if(host_->input_->isTriggerMouseButton(1)) {
		host_->currentState_.reset(new RotationState(host_));
		return;
	}
}

void DebugCamera::TranslationState::Update() {
	if(!host_->input_->isTriggerMouseButton(0) || !(host_->input_->isPressKey(DIK_LALT) || host_->input_->isPressKey(DIK_RALT))) {
		host_->currentState_.reset(new Neutral(host_));
		return;
	}
	Vector3 velo = {host_->input_->getMouseVelocity().x,host_->input_->getMouseVelocity().y,0.0f};
	host_->viewProj_.translate += velo * 0.01f;
}

void DebugCamera::RotationState::Update() {
	if(!host_->input_->isTriggerMouseButton(1) ||
	   !(host_->input_->isPressKey(DIK_LALT) ||
		 host_->input_->isPressKey(DIK_RALT))) {
		host_->currentState_.reset(new Neutral(host_));
		return;
	}
	host_->viewProj_.rotate += Vector3(host_->input_->getMouseVelocity().x,host_->input_->getMouseVelocity().y,0.0f) * 0.01f;
}
