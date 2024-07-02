#include "GameObject.h"

#include "System.h"
#include "imgui.h"

void GameObject::Init(const std::string &modelName,const std::string &directoryPath,const std::string &materialName,const Transform &transform) {
	model_ = Model::Create(directoryPath,modelName);

	materialName_ = materialName;
	material_ = System::getInstance()->getMaterialManager()->Create(materialName_);

	worldTransform_.Init();
	worldTransform_.transformData = transform;
	worldTransform_.Update();
}

void GameObject::Init(const std::string &modelName,const std::string &directoryPath,const std::string &materialName,MaterialManager *materialManager,const Transform &transform) {
	model_ = Model::Create(directoryPath,modelName);

	materialName_ = materialName;
	material_ = materialManager->Create(materialName_);

	worldTransform_.Init();
	worldTransform_.transformData = transform;
	worldTransform_.Update();
}

void GameObject::DebugUpdate(MaterialManager *materialManager) {
#ifdef _DEBUG
	ImGui::DragFloat3("Scale",&worldTransform_.transformData.scale.x,0.1f);
	ImGui::DragFloat3("Rotate",&worldTransform_.transformData.rotate.x,0.1f);
	ImGui::DragFloat3("Translate",&worldTransform_.transformData.translate.x,0.1f);
	worldTransform_.Update();

	ImGui::InputText("Material Changing to",inputMaterialName_,32);
	if(ImGui::Button("Change Material")) {
		// 入力された名前のマテリアルを取得
		std::shared_ptr<Material> newMaterial = materialManager->getMaterialShared(std::string(inputMaterialName_));

		if(newMaterial && material_ != newMaterial) {
			materialName_ = inputMaterialName_;
			material_ = newMaterial;
		}
	}
#endif // _DEBUG
}

void GameObject::Draw(const ViewProjection &viewProj) {
	model_->Draw(worldTransform_,viewProj,material_.get());
}