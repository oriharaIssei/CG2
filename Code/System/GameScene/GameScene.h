#pragma once

#include <DirectXCommon.h>

#include <Vector3.h>
#include <Matrix4x4.h>

class GameScene {
public:
	GameScene() = default;

	void Init();
	void Update();
	void Draw();
private:
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	Transform transform_;
	Transform cameraT_;
	
	int gh_;

	Matrix4x4 wvpVpMa_;
};