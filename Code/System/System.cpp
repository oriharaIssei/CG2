#include "System.h"

#include "Vector4.h"
#include "Matrix4x4.h"

std::unique_ptr<WinApp> System::window_ = nullptr;
std::unique_ptr<DirectXCommon> System::dxCommon_ = nullptr;
std::unique_ptr<ShaderCompiler> System::shaderCompiler_ = nullptr;
std::unique_ptr<PipelineStateObj> System::pso_ = nullptr;
std::unique_ptr<TriangleBufferVerw> System::triangle_ = nullptr;
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

	dxCommon_->CreateBufferResource(constantBuff_, sizeof(Vector4));
	dxCommon_->CreateBufferResource(wvpResource_, sizeof(Matrix4x4));

	Vector4* materialData = nullptr;
	constantBuff_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	*wvpData = MakeMatrix::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f });
}

void System::Finalize() {
	window_->TerminateGameWindow();

	dxCommon_->Finalize();
	shaderCompiler_->Finalize();
	pso_->Finalize();
	triangle_->Finalize();
}

void System::DrawTriangle(const Matrix4x4& wvp) {
	Vector4* vertDate = nullptr;
	
	triangle_->vertBuff_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&vertDate)
	);
	vertDate[0] = { -0.5f,-0.5f,0.0f,1.0f };
	vertDate[1] = { 0.0f,0.5f,0.0f,1.0f };
	vertDate[2] = { 0.5f,-0.5f,0.0f,1.0f };

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
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

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced(3, 1, 0, 0);
}

bool System::ProcessMessage() {
	return window_->ProcessMessage();
}

void System::BeginFrame() {
	dxCommon_->PreDraw();
}

void System::EndFrame() {
	dxCommon_->PostDraw();
}
