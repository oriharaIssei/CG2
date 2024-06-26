#include "GameObject.h"

#include "System.h"
#include "imgui.h"

void GameObject::Init(const std::string &modelName,const std::string &directoryPath,const std::string &materialName,const Transform &transform) {
	model_.reset(Model::Create(directoryPath,modelName + ".obj"));
	material_ = System::getInstance()->getMaterialManager()->Create(materialName);

	worldTransform_.Init();
	worldTransform_.transformData = transform;
	worldTransform_.Update();
}

void GameObject::DebugUpdate(MaterialManager *materialManager) {
#ifdef _DEBUG
	ImGui::DragFloat3("Scale",&worldTransform_.transformData.scale.x);
	ImGui::DragFloat3("Rotate",&worldTransform_.transformData.rotate.x);
	ImGui::DragFloat3("Translate",&worldTransform_.transformData.translate.x);
	worldTransform_.Update();

	ImGui::InputText("Material Changing to",inputMaterialName_,32);
	if(ImGui::Button("Change Material")) {
		try {
			// 入力された名前のマテリアルを取得
			std::shared_ptr<Material> newMaterial = materialManager->getMaterialShared(std::string(inputMaterialName_));

			// 取得したマテリアルが有効で、現在のマテリアルと異なる場合のみ変更
			if(newMaterial && material_ != newMaterial) {
				material_ = newMaterial;
			}
		} catch(const std::exception &e) {
			assert(false);
		}
	}

#endif // _DEBUG
}

void GameObject::Draw(const ViewProjection &viewProj) {
	model_->Draw(worldTransform_,viewProj,material_.get());
}