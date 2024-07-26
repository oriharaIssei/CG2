#pragma once
#include "IGameObject.h"

#include "Model.h"
#include "WorldTransform.h"

class ModelObject :
	public IGameObject{
public:
	void Init(const std::string &directryPath,const std::string &objectName)override;
	void Updata()override;
	void Draw(const ViewProjection &viewProj)override;
private:
	std::unique_ptr<WorldTransform> transform_;
	std::shared_ptr<Model> model_;
};