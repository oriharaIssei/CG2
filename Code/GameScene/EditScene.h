#pragma once
#include "IScene.h"

#include "MapEditor.h"

class EditScene :
	public IScene {
public:
	EditScene(SceneManager *host):IScene(host) {};

	void Init()override;
	void Update()override;
	void Draw()override;
private:
	std::unique_ptr<MapEditor> mapEditor_;
};