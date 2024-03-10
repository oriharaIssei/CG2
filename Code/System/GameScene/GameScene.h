#pragma once

#include <DirectXCommon.h>

class GameScene {
public:
	GameScene(DirectXCommon* dXCommon);

	void Init();
	void Update();
	void Draw();
private:
	DirectXCommon* dXCommon_;
};