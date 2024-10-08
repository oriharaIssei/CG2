#include "PointLight.h"

#include "DXFunctionHelper.h"
#include "System.h"

#include "ImGui.h"

void PointLight::Init(){
	mappingData_ = nullptr;

	DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),constBuff_,sizeof(PointLight::ConstBuffer));

	constBuff_->Map(
		0,nullptr,reinterpret_cast<void **>(&mappingData_)
	);
}

void PointLight::Finalize(){
	constBuff_.Reset();
}

void PointLight::DebugUpdate(){
#ifdef _DEBUG
	ImGui::Begin("PointLight");
	ImGui::DragFloat3("Position",&this->pos.x,0.01f);
	ImGui::ColorEdit4("Color",&this->color.x);
	ImGui::SliderFloat("Intensity",&this->intensity,0.0f,1.0f);
	ImGui::DragFloat("Radius",&this->radius,0.1f,0.1f,FLT_MAX);
	ImGui::DragFloat("Decay",&this->decay,0.1f,0.1f,FLT_MAX);
	this->ConvertToBuffer();
	ImGui::End();
#endif // _DEBUG
}

void PointLight::SetForRootParameter(ID3D12GraphicsCommandList *cmdList,UINT rootParameterNum) const{
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum,constBuff_->GetGPUVirtualAddress());
}

void PointLight::ConvertToBuffer(){
	mappingData_->color     = this->color;
	mappingData_->pos       = this->pos;
	mappingData_->intensity = this->intensity;
	mappingData_->radius    = this->radius;
	mappingData_->decay     = this->decay;
}
