#pragma once
#pragma once

#include <Input.h>

#include "GameObject/IGameObject.h"

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

	std::list<std::unique_ptr<IGameObject>> gameObjects_;
	std::list<std::pair<std::string,std::string>> textureList_;
	std::list<std::pair<std::string,std::string>> objectList_;

	std::unique_ptr<MaterialManager> materialManager_;
};