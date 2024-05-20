#pragma once

#include <DirectXCommon.h>

#include "Sprite/Sprite.h"
#include <Model.h>

#include <WorldTransform.h>
#include <ViewProjection.h>

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

	Transform camera;
	Transform uv;
	Transform sphere;

	ViewProjection viewProj_;
	WorldTransform worldTransform_;
	std::unique_ptr<Sprite> model;
};