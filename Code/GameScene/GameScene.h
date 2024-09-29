#pragma once

#include <list>
#include <memory>

#include <string>

#include "Audio/Audio.h"
#include "DebugCamera.h"
#include "ViewProjection.h"

#include "GameObject/IGameObject.h"
#include "RenderTexture.h"

class Input;
class MaterialManager;
class DXRtvArray;
class DXSrvArray;

struct Matrix4x4;
struct Vector3;

class GameScene{
public:
	GameScene() = default;
	~GameScene();

	void Init();
	void Update();
	void Draw();
private:
	std::unique_ptr<DebugCamera> debugCamera_;
	ViewProjection viewProj_;
	Input* input_;

	std::shared_ptr<DXRtvArray> sceneRtvArray_;
	std::shared_ptr<DXSrvArray> sceneSrvArray_;
	std::unique_ptr<RenderTexture> sceneView_;

	std::list<std::unique_ptr<IGameObject>> gameObjects_;
	std::list<std::pair<std::string,std::string>> textureList_;
	std::list<std::pair<std::string,std::string>> objectList_;

	std::unique_ptr<Audio> audio_;

	MaterialManager* materialManager_;
};