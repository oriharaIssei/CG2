#pragma once

#include <memory>

#include <string>

#include "Material.h"
#include "Model.h"

#include "Transform.h"

/////////////////////////////////////////////
/// 今回はモデルしか使わないが，
/// ModelTypeとPrimitiveTypeの両方を作るべき
/////////////////////////////////////////////
class GameObject {
public:
	void Init(const std::string &modelName,const std::string &directoryPath,const std::string &materialName,const Transform &transform);
	void DebugUpdate();
	void Draw(const ViewProjection &viewProj);
private:
	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;
	std::shared_ptr<Material> material_;

#ifdef _DEBUG
	char inputMaterialName_[32];
#endif // _DEBUG


public:
	const WorldTransform &getWorldTransform()const { return worldTransform_; }
	void setTransformParent(WorldTransform *newParent) {
		worldTransform_.parent = newParent;
		worldTransform_.Update();
	}
	const Model *getModel()const { return model_.get(); }
};