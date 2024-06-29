#pragma once

#include <memory>

#include "Input.h"

#include "Model.h"
#include "Sprite.h"

#include <ViewProjection.h>

#include <DebugCamera.h>

class SceneManager;
class IScene {
public:
	IScene(SceneManager *host):host_(host) {};
	virtual ~IScene() {};

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
protected:
	SceneManager *host_;

#ifdef _DEBUG
	std::unique_ptr<DebugCamera> debugCamera_;
#endif // _DEBUG
	Input *input_;
	ViewProjection viewProj_;
};