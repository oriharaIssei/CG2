#include "WorldTransform.h"

#include <System.h>
#include "imgui.h"
#include "DXFunctionHelper.h"

void WorldTransform::Init(){
	DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),buff_,sizeof(ConstantBufferWorldMatrix));
	buff_->Map(0,nullptr,reinterpret_cast<void **>(&mappingWorldMat_));

	worldMat = MakeMatrix::Identity();
	mappingWorldMat_->world = worldMat;
}

void WorldTransform::Finalize(){
	buff_.Reset();
}

void WorldTransform::Update(){
	worldMat = MakeMatrix::Affine(scale,rotate,translate);

	if(parent != nullptr){
		worldMat *= parent->worldMat;
	}
	ConvertToBuffer();
}

void WorldTransform::Debug(const std::string &transformName){
	std::string labelName = transformName + " scale";
	ImGui::DragFloat3(labelName.c_str(),&scale.x,0.01f);
	labelName = transformName + " rotate";
	ImGui::DragFloat3(labelName.c_str(),&rotate.x,0.01f);
	labelName = transformName + " translation";
	ImGui::DragFloat3(labelName.c_str(),&translate.x,0.1f);
}

void WorldTransform::ConvertToBuffer(){
	mappingWorldMat_->world = worldMat;
}

void WorldTransform::SetForRootParameter(ID3D12GraphicsCommandList *cmdList,UINT rootParameterNum) const{
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum,buff_->GetGPUVirtualAddress());
}
