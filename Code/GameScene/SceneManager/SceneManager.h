#pragma once

#include "IScene.h"

#include <memory>

class SceneManager {
public:
	void Init(IScene *start);
	void Run();
	void TransitionScene(IScene *next);
private:
	std::unique_ptr<IScene> currentScene_;
};