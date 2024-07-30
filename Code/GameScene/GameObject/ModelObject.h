#pragma once
#include "IGameObject.h"

#include "Model.h"
#include "WorldTransform.h"

class ModelObject :
	public IGameObject{
public:
	~ModelObject()override{};
	void Init(const std::string &directryPath,const std::string &objectName)override;
	void Updata()override;
	void Draw(const ViewProjection &viewProj)override;
private:
	std::shared_ptr<Model> model_;
};