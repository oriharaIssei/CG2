#pragma once

#include "IGameObject.h"

#include <memory>

#include "ViewProjection.h"
#include "WorldTransform.h"

class SphereObject :
	public IGameObject{
public:
	~SphereObject()override{};

	void Init([[maybe_unused]] const std::string &directryPath,const std::string &objectName)override;
	void Updata()override;
	void Draw(const ViewProjection &viewProj)override;
private:
};