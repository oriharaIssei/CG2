#pragma once
#include "IGameObject.h"

#include "Sprite.h"

#include "Vector2.h"

class SpriteObject :
	public IGameObject{
public:
	void Init(const std::string &objectName)override;
	void Updata()override;
	void Draw(const ViewProjection &viewProj)override;
private:
	std::unique_ptr<Sprite> sprite_;

	Vector2 pos_,size_;
};