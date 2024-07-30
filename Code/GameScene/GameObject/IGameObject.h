#pragma once

#include <memory>

#include <list>

#include <string>

#include "Material.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class IGameObject{
public:
	virtual ~IGameObject(){};

	virtual void Init(const std::string &directryPath,const std::string &objectName);
	virtual void Updata();
	virtual void Draw(const ViewProjection &viewProj) = 0;
protected:
	static std::list<std::pair<std::string,std::string>> textureList_;
	int currentTextureNum_;

	MaterialManager *materialManager_;
	Material *material_;
	char materialName[32];

	[[maybe_unused]] WorldTransform transform_;

	std::string name_;
public:
	void setMaterialManager(MaterialManager *materialManager){ materialManager_ = materialManager; }
};