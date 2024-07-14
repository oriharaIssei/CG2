#pragma once

#include <Input.h>

#include "Sprite.h"
#include <Model.h>
#include "Particle/Particle.h"

#include <ViewProjection.h>
#include <WorldTransform.h>

#include <Matrix4x4.h>
#include <Transform.h>
#include <Vector3.h>

#include "Material.h"

#include <DebugCamera.h>

class GameScene{
public:
	GameScene() = default;
	~GameScene();

	void Init();
	void Update();
	void Draw();
private:
	DebugCamera debugCamera;
	ViewProjection viewProj_;
	Input *input_;

	std::unique_ptr<MaterialManager> materialManager_;
};