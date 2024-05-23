#pragma once

#include <DirectXCommon.h>
#include <Input.h>

#include "Sprite.h"
#include <Model.h>

#include <ViewProjection.h>
#include <WorldTransform.h>

#include <DebugCamera.h>

#include <Matrix4x4.h>
#include <Transform.h>
#include <Vector3.h>

class GameScene {
public:
	GameScene() = default;
	~GameScene();

	void Init();
	void Update();
	void Draw();
private:
	DebugCamera debugCamera;
	Input *input_;

	Transform camera;
	Transform uv;
	Transform sphere;

	ViewProjection viewProj_;
	WorldTransform worldTransform_;
};