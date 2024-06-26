#pragma once

#include <Input.h>

#include "Model.h"
#include "Sprite.h"

#include <ViewProjection.h>
#include <WorldTransform.h>

#include <DebugCamera.h>

#include <Matrix4x4.h>
#include <Transform.h>
#include <Vector3.h>

#include "MapEditor.h"

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

	ViewProjection viewProj_;

	std::unique_ptr<MapEditor> mapEditor_;
};