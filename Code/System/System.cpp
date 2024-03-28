#include "System.h"

#include <ImGuiManager.h>
#include <TextureManager.h>

std::unique_ptr<WinApp> System::window_ = nullptr;
std::unique_ptr<DirectXCommon> System::dxCommon_ = nullptr;
std::unique_ptr<ShaderCompiler> System::shaderCompiler_ = nullptr;
std::unique_ptr<PipelineStateObj> System::pso_ = nullptr;
std::unique_ptr<TriangleBufferVerw> System::triangle_ = nullptr;
std::unique_ptr<Splite> System::splite_ = nullptr;

Microsoft::WRL::ComPtr<ID3D12Resource> System::constantBuff_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> System::wvpResource_ = nullptr;

void System::Init() {
	window_ = std::make_unique<WinApp>();
	window_->CreateGameWindow(L"title", WS_OVERLAPPEDWINDOW, 1280, 720);

	dxCommon_ = std::make_unique<DirectXCommon>(window_.get());
	dxCommon_->Init();

	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Init();

	pso_ = std::make_unique<PipelineStateObj>();
	pso_->Init(dxCommon_.get());

	triangle_ = std::make_unique<TriangleBufferVerw>();
	triangle_->Init(dxCommon_.get());

	splite_ = std::make_unique<Splite>();
	splite_->Init(dxCommon_.get());

	dxCommon_->CreateBufferResource(constantBuff_, sizeof(Vector4));
	dxCommon_->CreateBufferResource(wvpResource_, sizeof(Matrix4x4));

	TextureManager::Init(dxCommon_.get());

	ImGuiManager::getInstance()->Init(window_.get(), dxCommon_.get());

	Vector4* materialData = nullptr;
	constantBuff_->Map(0, nullptr, reinterpret_cast<void**>( &materialData ));
	*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>( &wvpData ));
	*wvpData = MakeMatrix::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f });
}

void System::Finalize() {
	ImGuiManager::getInstance()->Finalize();

	window_->TerminateGameWindow();
	dxCommon_->Finalize();

	shaderCompiler_->Finalize();
	pso_->Finalize();
	triangle_->Finalize();
	splite_->Finalize();
	TextureManager::Finalize();

	constantBuff_.Reset();
	wvpResource_.Reset();

	DirectXCommon::CheckIsAliveInstance();
}

void System::DrawTriangle(const Matrix4x4& wvp) {
	VertexData* vertData = nullptr;

	triangle_->vertBuff_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>( &vertData )
	);

	vertData[0].pos = { -0.5f,-0.5f,0.0f,1.0f };

	vertData[1].pos = { 0.0f,0.5f,0.0f,1.0f };

	vertData[2].pos = { 0.5f,-0.5f,0.0f,1.0f };

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>( &wvpData ));
	*wvpData = wvp;

	dxCommon_->getCommandList()->SetGraphicsRootSignature(pso_->rootSignature_.Get());
	dxCommon_->getCommandList()->SetPipelineState(pso_->pipelineState_.Get());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &triangle_->vbView);
	//形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		constantBuff_->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, //RootParameter配列の 1 番目
		constantBuff_->GetGPUVirtualAddress()
	);

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced(3, 1, 0, 0);
}

void System::DrawTexture(const Matrix4x4& wvp, int textureNum) {
	VertexData* vertData = nullptr;

	triangle_->vertBuff_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>( &vertData )
	);

	vertData[0].pos = { -0.5f,-0.5f,0.0f,1.0f };
	vertData[0].texCoord = { 0.0f,1.0f };

	vertData[1].pos = { 0.0f,0.5f,0.0f,1.0f };
	vertData[1].texCoord = { 0.5,0.0f };

	vertData[2].pos = { 0.5f,-0.5f,0.0f,1.0f };
	vertData[2].texCoord = { 1.0f,1.0f };

	vertData[3].pos = { -0.5f,-0.5f,0.5f,1.0f };
	vertData[3].texCoord = { 0.0f,1.0f };

	vertData[4].pos = { 0.0f,0.0f,0.0f,1.0f };
	vertData[4].texCoord = { 0.5f,0.0f };

	vertData[5].pos = { 0.5f,-0.5f,-0.5f,1.0f };
	vertData[5].texCoord = { 1.0f,1.0f };

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>( &wvpData ));
	*wvpData = wvp;

	dxCommon_->getCommandList()->SetGraphicsRootSignature(pso_->rootSignature_.Get());
	dxCommon_->getCommandList()->SetPipelineState(pso_->pipelineState_.Get());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &triangle_->vbView);
	//形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		constantBuff_->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, //RootParameter配列の 0 番目
		wvpResource_->GetGPUVirtualAddress()
	);

	ID3D12DescriptorHeap* ppHeaps[] = { dxCommon_->getSrvDescriptorHeap() };
	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);

	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
		2,
		TextureManager::getGpuHandle(textureNum)
	);

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced(6, 1, 0, 0);
}

void System::DrawSprite(const Matrix4x4& wvp, int textureNum) {
	VertexData* vertData = nullptr;

	splite_->vertBuff_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>( &vertData )
	);

	vertData[0].pos = { 0.0f,360.0f,0.0f,1.0f };
	vertData[0].texCoord = { 0.0f,1.0f };
	vertData[1].pos = { 0.0f,0.0f,0.0f,1.0f };
	vertData[1].texCoord = { 0.0,0.0f };
	vertData[2].pos = { 640.0f,360.0f,0.0f,1.0f };
	vertData[2].texCoord = { 1.0f,1.0f };

	vertData[3].pos = { 0.0f,0.0f,0.0f,1.0f };
	vertData[3].texCoord = { 0.0f,0.0f };
	vertData[4].pos = { 640.0f,0.0f,0.0f,1.0f };
	vertData[4].texCoord = { 1.0f,0.0f };
	vertData[5].pos = { 640.0f,360.0f,0.0f,1.0f };
	vertData[5].texCoord = { 1.0f,1.0f };

	Matrix4x4* wvpData = nullptr;
	splite_->matrixBuff_->Map(0, nullptr, reinterpret_cast<void**>( &wvpData ));
	*wvpData = wvp;

	dxCommon_->getCommandList()->SetGraphicsRootSignature(pso_->rootSignature_.Get());
	dxCommon_->getCommandList()->SetPipelineState(pso_->pipelineState_.Get());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &splite_->vbView_);
	// 形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		constantBuff_->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, 
		splite_->matrixBuff_->GetGPUVirtualAddress()
	);

	ID3D12DescriptorHeap* ppHeaps[] = { dxCommon_->getSrvDescriptorHeap() };
	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);

	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
		2,
		TextureManager::getGpuHandle(textureNum)
	);

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced(6, 1, 0, 0);
}

bool System::ProcessMessage() {
	return window_->ProcessMessage();
}

void System::BeginFrame() {
	ImGuiManager::getInstance()->Begin();
	dxCommon_->PreDraw();
}

void System::EndFrame() {
	ImGuiManager::getInstance()->End();
	ImGuiManager::getInstance()->Draw(dxCommon_.get());
	dxCommon_->PostDraw();
}

int System::LoadTexture(const std::string& filePath) {
	return TextureManager::LoadTexture(filePath);
}
