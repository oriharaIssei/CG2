#pragma once

#include "IScene.h"

#include "GameMap.h"
#include "Player.h"

class GameScene
	:public IScene {
public:
	GameScene(SceneManager *host):IScene(host) {};

	void Init()override;
	void Update()override;
	void Draw()override;
private:
	std::unique_ptr<GameMap> gameMap_;
	std::unique_ptr<Player> player_;
};