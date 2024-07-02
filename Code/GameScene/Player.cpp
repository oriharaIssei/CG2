#include "Player.h"

#include "System.h"
#include <imgui.h>

void Player::Init(const Vector3 &pos) {
	transform_.Init();
	transform_.transformData.translate = pos;

	speed_ = 0.1f;
	eyeSpeed_ = 0.008f;

	input_ = Input::getInstance();
}

void Player::Update() {
	if(!input_->isPressKey(DIK_LALT)) {
		System::getInstance()->setMousePos({680.0f,360.0f});
		Vector3 eyeDir = {input_->getCurrentMousePos() - Vector2(680.0f,360.0f),0.0f};

		eyeDir = {eyeDir.y,eyeDir.x,0.0f};

		transform_.transformData.rotate += eyeDir * eyeSpeed_;
	}

	Vector3 dir;

	dir = {static_cast<float>(input_->isPressKey(DIK_D) - input_->isPressKey(DIK_A)),
		0,
		static_cast<float>(input_->isPressKey(DIK_W) - input_->isPressKey(DIK_S))};

	dir = TransformVector(dir,MakeMatrix::RotateXYZ(transform_.transformData.rotate));

	transform_.transformData.translate += dir * speed_;

	transform_.Update();
}