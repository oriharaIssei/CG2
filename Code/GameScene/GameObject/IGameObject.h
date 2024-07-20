#pragma once

#include <memory>

#include <list>

#include <string>

#include "Material.h"
#include "ViewProjection.h"

class IGameObject{
public:
	virtual void Init(const std::string &objectName);
	virtual void Updata() = 0;
	virtual void Draw(const ViewProjection &viewProj) = 0;
protected:
	int currentTextureNum_;
	static std::list<std::pair<std::string,std::string>> textureList_;
	MaterialManager *materialManager_;
	Material *material_;

	std::string name_;
public:
	void setMaterialManager(MaterialManager *materialManager){ materialManager_ = materialManager; }
};