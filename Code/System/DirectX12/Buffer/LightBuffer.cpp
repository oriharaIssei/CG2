#include "LightBuffer.h"

#include "DXFunctionHelper.h"
#include "System.h"

void LightBuffer::Init() {
	directionalLigh_ = nullptr;

	DXFH::CreateBufferResource(System::getInstance()->getDXDevice(),constBuff_, sizeof(DirectionalLight));

	constBuff_->Map(
		0, nullptr, reinterpret_cast<void **>(&directionalLigh_)
	);
}

void LightBuffer::Finalize() {
	constBuff_.Reset();
}

void LightBuffer::SetForRootParameter(ID3D12GraphicsCommandList *cmdList, UINT rootParameterNum)const {
	cmdList->SetGraphicsRootConstantBufferView(rootParameterNum, constBuff_->GetGPUVirtualAddress());
}

void LightBuffer::ConvertToBuffer() {
	directionalLigh_->color = color;
	directionalLigh_->direction = direction;
	directionalLigh_->intensity = intensity;
}