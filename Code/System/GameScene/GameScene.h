#pragma once

#include <DirectXCommon.h>

#include <Model.h>

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

	std::unique_ptr<Model> model;
};