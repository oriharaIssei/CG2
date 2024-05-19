#include "Sprite.h"

#include "System.h"
#include "TextureManager.h"


Sprite *Sprite::Create(const Vector2 &size, const std::string &textureFilePath) {
	Sprite *result = new Sprite();
	;
	result->size_ = size;

	result->th_ = TextureManager::LoadTexture(textureFilePath);

	result->meshBuff_ = std::make_unique<TextureObject3dMesh>();

	result->meshBuff_->vertData[0] = { {0.0f,size.y,0.0f,1.0f},{0.0f,1.0f},{0.0f, 0.0f,-1.0f} };
	result->meshBuff_->vertData[1] = { {0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},{0.0f,0.0f,-1.0f} };
	result->meshBuff_->vertData[2] = { {size.x, size.y,0.0f,1.0f},{1.0f,1.0f},{0.0f, 0.0f,-1.0f} };
	result->meshBuff_->vertData[3] = { {size.x,0.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f, 0.0f,-1.0f} };

	result->meshBuff_->indexData[0] = 0;
	result->meshBuff_->indexData[1] = 1;
	result->meshBuff_->indexData[2] = 2;
	result->meshBuff_->indexData[3] = 1;
	result->meshBuff_->indexData[4] = 3;
	result->meshBuff_->indexData[5] = 2;

	result->meshBuff_->Create(6);

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

void Sprite::Draw(const WorldTransform &world, const ViewProjection &view) {
	DirectXCommon *dxCommon_ = System::getInstance()->getDxCommon();

	dxCommon_->getCommandList()->IASetIndexBuffer(&meshBuff_->ibView);
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &meshBuff_->vbView);

	world.SetForRootParameter(System::getInstance()->getDxCommon()->getCommandList(), 0);
	view.SetForRootParameter(System::getInstance()->getDxCommon()->getCommandList(), 1);

	dxCommon_->getCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}