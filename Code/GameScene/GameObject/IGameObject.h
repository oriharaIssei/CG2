#pragma once

#include <memory>

#include <string>

#include "ViewProjection.h"
#include "Material.h"
#include "WorldTransform.h"

class IGameObject{
public:
	virtual void Init(const std::string &objectName);
	virtual void Updata();
	virtual void Draw(const ViewProjection &viewProj) = 0;
protected:
	std::string name_;
	MaterialManager *materialManager_;
	std::unique_ptr<WorldTransform> transform_;
public:
	void setMaterialManager(MaterialManager *materialManager){ materialManager_ = materialManager; }
};