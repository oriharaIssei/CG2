#include "DebugCamera.h"

#include <imgui.h>

void DebugCamera::Init() {
	input_ = Input::getInstance();
	currentState_.reset(new Neutral(this));
}

void DebugCamera::Init(const Vector3&viewRotate,const Vector3&viewPos) {
	viewProj_.rotate= viewRotate;
	viewProj_.translate=viewPos;

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
	if(host_->input_->isTriggerMouseButton(0) || host_->input_->isWheel()) {
		host_->currentState_.reset(new TranslationState(host_));
		return;
	} else if(host_->input_->isTriggerMouseButton(1)) {
		host_->currentState_.reset(new RotationState(host_));
		return;
	}
}

void DebugCamera::TranslationState::Update() {
	uint32_t state = 0;
	bool a = host_->input_->isPreWheel();
	bool b = host_->input_->isPressMouseButton(0);
	uint32_t c = (host_->input_->isPressKey(DIK_LALT) | host_->input_->isPressKey(DIK_RALT));
	state = (a)+(b * 2);
	state *= c;
	Vector3 velo = {};
	switch((TranslationType)state) {
	case NONE:
	host_->currentState_.reset(new Neutral(host_));
	return;
	case Z_WHEEL:
	velo = {0.0f,0.0f,(float)host_->input_->getPreWheel() * 0.007f};
	break;
	case XY_MOUSEMOVE:
	velo = {host_->input_->getMouseVelocity() * 0.01f,0.0f};
	break;
	case XYZ_ALL:
	velo = {host_->input_->getMouseVelocity() * 0.01f,(float)host_->input_->getPreWheel() * 0.007f};
	break;
	default:
	break;
	}
	velo.y *= -1.0f;
	host_->viewProj_.translate += velo;

}

void DebugCamera::RotationState::Update() {
	if(!host_->input_->isPressMouseButton(1) ||
	   !(host_->input_->isPressKey(DIK_LALT) ||
		 host_->input_->isPressKey(DIK_RALT))) {
		host_->currentState_.reset(new Neutral(host_));
		return;
	}
	host_->viewProj_.rotate += Vector3(host_->input_->getMouseVelocity().y,host_->input_->getMouseVelocity().x,0.0f) * 0.01f;
}
