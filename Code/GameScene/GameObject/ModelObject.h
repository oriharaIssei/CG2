#pragma once
#include "IGameObject.h"

#include "Model.h"

class ModelObject :
	public IGameObject{
public:
	void Init(const std::string &objectName)override;
	void Updata()override;
	void Draw(const ViewProjection &viewProj)override;
private:
	
	std::shared_ptr<Model> model_;
};