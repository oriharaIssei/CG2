#include "Sprite.h"

#include "System.h"
#include "TextureManager.h"

Matrix4x4 Sprite::orthoMa_;

void Sprite::Init() {
	orthoMa_ = MakeMatrix::Orthographic(0.0f, 0.0f, System::getInstance()->getWinApp()->getWidth(), System::getInstance()->getWinApp()->getHeight(), 0.0f, 100.0f);
}

Sprite *Sprite::Create(const Vector2 &pos, const Vector2 &size, const std::string &textureFilePath) {
	Sprite *result = new Sprite();

	result->pos_ = pos;
	result->size_ = size;

	result->th_ = TextureManager::LoadTexture(textureFilePath);

	result->buffer_ = std::make_unique<CommonBuffer>();
	result->buffer_->Create(System::getInstance()->getDxCommon(), true, ShapeType::QUAD);

	result->buffer_->textureVertData[0] = { {0.0f,size.y,0.0f,1.0f},{0.0f,1.0f},{0.0f, 0.0f,-1.0f} };
	result->buffer_->textureVertData[1] = { {0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},{0.0f,0.0f,-1.0f} };
	result->buffer_->textureVertData[2] = { {size.x, size.y,0.0f,1.0f},{1.0f,1.0f},{0.0f, 0.0f,-1.0f} };
	result->buffer_->textureVertData[3] = { {size.x,0.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f, 0.0f,-1.0f} };

	return result;
}

void Sprite::PreDraw() {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();
	dxCommon_->getCommandList()->SetGraphicsRootSignature(System::getInstance()->getTexturePso()->rootSignature.Get());
	dxCommon_->getCommandList()->SetPipelineState(System::getInstance()->getTexturePso()->pipelineState.Get());
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
}

void Sprite::Draw(const Matrix4x4 &world, const Matrix4x4 &view) {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();

	*buffer_->matrixData = { {
			world *
			view *
			orthoMa_
		},
		{world}
	};

	dxCommon_->getCommandList()->IASetIndexBuffer(&buffer_->ibView);
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &buffer_->vbView);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		CommonBuffer::materialBuff->GetGPUVirtualAddress()
	); 
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, //RootParameter配列の 1 番目
		CommonBuffer::matrixBuff->GetGPUVirtualAddress()
	);
	

	ID3D12DescriptorHeap *ppHeaps[] = { dxCommon_->getSrv() };
	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);
	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
		3,
		TextureManager::getGpuHandle(th_)
	);

	// 描画!!!
	dxCommon_->getCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}