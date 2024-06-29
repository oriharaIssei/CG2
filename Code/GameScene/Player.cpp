#include "Player.h"

#include <imgui.h>

void Player::Init(const Vector3 &pos) {
	transform_.Init();
	transform_.transformData.translate = pos;

	speed_ = 0.01f;

	input_ = Input::getInstance();
}

void Player::Update() {
	Vector3 dir;

	ImGui::DragFloat("Player Speed",&speed_,0.0f,3.0f);

	dir = {static_cast<float>(input_->isPressKey(DIK_D) - input_->isPressKey(DIK_A)),
		0,
		static_cast<float>(input_->isPressKey(DIK_W) - input_->isPressKey(DIK_S))};
	transform_.transformData.translate += dir * speed_;

	transform_.Update();
}