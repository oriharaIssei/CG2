#include "LightBuffer.h"

#include "System.h"

void LightBuffer::Init() {
	directionalLigh_ = nullptr;
	constBuff_->Map(
		0, nullptr, reinterpret_cast<void **>(&directionalLigh_)
	);
}

void LightBuffer::SetConstantBuffer() {
	System::getInstance()->getDxCommon()->getCommandList()->SetGraphicsRootConstantBufferView(
		2,
		constBuff_->GetGPUVirtualAddress()
	);
}

void LightBuffer::Convert2Buffer() {
	directionalLigh_->color = color;
	directionalLigh_->direction = direction;
	directionalLigh_->intensity = intensity;
}
