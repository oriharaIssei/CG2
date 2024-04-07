#include "System.h"

#include <ImGuiManager.h>
#include <Logger.h>
#include <TextureManager.h>
#include <imgui.h>

#define _USE_MATH_DEFINES
#include <cmath>

std::unique_ptr<WinApp> System::window_ = nullptr;
std::unique_ptr<DirectXCommon> System::dxCommon_ = nullptr;
std::unique_ptr<ShaderCompiler> System::shaderCompiler_ = nullptr;
std::unique_ptr<PipelineStateObj> System::primitivePSO_ = nullptr;
std::unique_ptr<PrimitiveBuffer> System::triangle_ = nullptr;
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

	primitivePSO_ = std::make_unique<PipelineStateObj>();
	CreatePrimitivePSO();

	triangle_ = std::make_unique<PrimitiveBuffer>();
	triangle_->Init(dxCommon_.get(), 300, 3);

	splite_ = std::make_unique<Splite>();
	splite_->Init(dxCommon_.get());

	dxCommon_->CreateBufferResource(wvpResource_, sizeof(Matrix4x4));

	TextureManager::Init(dxCommon_.get());

	ImGuiManager::getInstance()->Init(window_.get(), dxCommon_.get());

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>( &wvpData ));
	*wvpData = MakeMatrix::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f });
}

void System::Finalize() {
	ImGuiManager::getInstance()->Finalize();

	window_->TerminateGameWindow();
	dxCommon_->Finalize();

	shaderCompiler_->Finalize();
	primitivePSO_->Finalize();
	triangle_->Finalize();
	splite_->Finalize();
	TextureManager::Finalize();

	constantBuff_.Reset();
	wvpResource_.Reset();

	DirectXCommon::CheckIsAliveInstance();
}

void System::DrawTriangle(const Vector3& position1, const Vector3& position2, const Vector3& position3, const Vector3& scale, Vector4 color) {
	size_t indexVertex = triangle_->index * triangle_->vertexNum;

	triangle_->vertData[indexVertex].pos = { position1.x,position1.y,position1.z,1.0f };
	triangle_->vertData[indexVertex].color = color;
	triangle_->vertData[indexVertex + 1].pos = { position2.x,position2.y,position2.z,1.0f };
	triangle_->vertData[indexVertex + 1].color = color;
	triangle_->vertData[indexVertex + 2].pos = { position3.x,position3.y,position3.z,1.0f };
	triangle_->vertData[indexVertex + 2].color = color;

	dxCommon_->getCommandList()->SetGraphicsRootSignature(primitivePSO_->rootSignature.Get());
	dxCommon_->getCommandList()->SetPipelineState(primitivePSO_->pipelineState.Get());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &triangle_->vbView);
	//形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, //RootParameter配列の 1 番目
		wvpResource_->GetGPUVirtualAddress()
	);

	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced(triangle_->vertexNum, 1, indexVertex, 0);
	++triangle_->index;
}

void System::DrawTexture(const Vector3& position1, const Vector3& position2, const Vector3& position3, const Vector3& scale, const Matrix4x4& wvp, Vector4* color, int textureNum) {
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

	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>( &wvpData ));
	*wvpData = wvp;

	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
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
	dxCommon_->getCommandList()->DrawInstanced(3, 1, 0, 0);
}

void System::DrawSprite(const Vector3& ltPos, const Vector2& size, int textureNum) {
	splite_->vertData[0].pos = { ltPos.x,ltPos.y + size.y,ltPos.z,1.0f };
	splite_->vertData[0].texCoord = { 0.0f,1.0f };
	splite_->vertData[1].pos = { ltPos.x,ltPos.y,ltPos.z,1.0f };
	splite_->vertData[1].texCoord = { 0.0,0.0f };
	splite_->vertData[2].pos = { ltPos.x + size.x,ltPos.y + size.y,ltPos.z,1.0f };
	splite_->vertData[2].texCoord = { 1.0f,1.0f };

	splite_->vertData[3].pos = { ltPos.x,ltPos.y,ltPos.z,1.0f };
	splite_->vertData[3].texCoord = { 0.0f,0.0f };
	splite_->vertData[4].pos = { ltPos.x + size.x,ltPos.y,ltPos.z,1.0f };
	splite_->vertData[4].texCoord = { 1.0f,0.0f };
	splite_->vertData[5].pos = { ltPos.x + size.x,ltPos.y + size.y,ltPos.z,1.0f };
	splite_->vertData[5].texCoord = { 1.0f,1.0f };

	splite_->materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	splite_->matrixData->wvp = MakeMatrix::Identity();

	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &splite_->vbView);
	// 形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		splite_->materialBuff->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1,
		splite_->matrixBuff->GetGPUVirtualAddress()
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

void System::DrawSphere(const Matrix4x4& world, const Matrix4x4& view, int textureNum) {
	constexpr int kSubDivision = 16;
	const float kLonEvery = static_cast<float>( M_PI ) * 2.0f / static_cast<float>( kSubDivision );
	const float kLatEvery = static_cast<float>( M_PI ) / static_cast<float>( kSubDivision );

	for(int latIndex = 0; latIndex < kSubDivision; ++latIndex) {
		float lat = -static_cast<float>( M_PI ) / 2.0f + ( kLatEvery * latIndex );
		for(int lonIndex = 0; lonIndex < kSubDivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			uint32_t startIndex = ( latIndex * kSubDivision + lonIndex ) * 6;

			// lb 0,0
			splite_->vertData[startIndex].pos = { Vector4(
				std::cosf(lat) * std::cosf(lon),
				std::sinf(lat),
				std::cosf(lat) * std::sinf(lon),
				1.0f)
			};
			splite_->vertData[startIndex].texCoord = {
				float(lonIndex) / float(kSubDivision),
				1.0f - float(latIndex) / float(kSubDivision)
			};
			splite_->vertData[startIndex].normal = { 
				splite_->vertData[startIndex].pos.x,
				splite_->vertData[startIndex].pos.y ,
				splite_->vertData[startIndex].pos.z 
			};

			// lt 0,1
			splite_->vertData[startIndex + 1].pos = { Vector4(
				std::cosf(lat + kLatEvery) * std::cosf(lon),
				std::sinf(lat + kLatEvery),
				std::cosf(lat + kLatEvery) * std::sinf(lon),
				1.0f)
			};
			splite_->vertData[startIndex + 1].texCoord = {
				float(lonIndex) / float(kSubDivision),
				1.0f - float(latIndex + 1.0f) / float(kSubDivision)
			};
			splite_->vertData[startIndex + 1].normal = {
				splite_->vertData[startIndex + 1].pos.x,
				splite_->vertData[startIndex + 1].pos.y ,
				splite_->vertData[startIndex + 1].pos.z
			};

			//rb 1,0
			splite_->vertData[startIndex + 2].pos = { Vector4(
				std::cosf(lat) * std::cosf(lon + kLonEvery),
				std::sinf(lat),
				std::cosf(lat) * std::sinf(lon + kLonEvery),
				1.0f)
			};
			splite_->vertData[startIndex + 2].texCoord = {
				float(lonIndex + 1.0f) / float(kSubDivision) ,
				1.0f - float(latIndex) / float(kSubDivision)
			};
			splite_->vertData[startIndex + 2].normal = {
				splite_->vertData[startIndex + 2].pos.x,
				splite_->vertData[startIndex + 2].pos.y ,
				splite_->vertData[startIndex + 2].pos.z
			};

			// lt 0,1
			splite_->vertData[startIndex + 3].pos = { Vector4(
				std::cosf(lat + kLatEvery) * std::cosf(lon),
				std::sinf(lat + kLatEvery),
				std::cosf(lat + kLatEvery) * std::sinf(lon),
				1.0f)
			};
			splite_->vertData[startIndex + 3].texCoord = {
				float(lonIndex) / float(kSubDivision),
				1.0f - float(latIndex + 1.0f) / float(kSubDivision)
			};
			splite_->vertData[startIndex + 3].normal = {
				splite_->vertData[startIndex + 3].pos.x,
				splite_->vertData[startIndex + 3].pos.y ,
				splite_->vertData[startIndex + 3].pos.z
			};

			// rt 1,1
			splite_->vertData[startIndex + 4].pos = { Vector4(
				std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
				std::sinf(lat + kLatEvery),
				std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),
				1.0f)
			};
			splite_->vertData[startIndex + 4].texCoord = {
				float(lonIndex + 1.0f) / float(kSubDivision) ,
				1.0f - float(latIndex + 1.0f) / float(kSubDivision)
			};
			splite_->vertData[startIndex + 4].normal = {
				splite_->vertData[startIndex + 4].pos.x,
				splite_->vertData[startIndex + 4].pos.y ,
				splite_->vertData[startIndex + 4].pos.z
			};

			// rb 1,0
			splite_->vertData[startIndex + 5].pos = { Vector4(
				std::cosf(lat) * std::cosf(lon + kLonEvery),
				std::sinf(lat),
				std::cosf(lat) * std::sinf(lon + kLonEvery),
				1.0f)
			};
			splite_->vertData[startIndex + 5].texCoord = {
				float(lonIndex + 1.0f) / float(kSubDivision) ,
				1.0f - float(latIndex) / float(kSubDivision)
			};
			splite_->vertData[startIndex + 5].normal = {
				splite_->vertData[startIndex + 5].pos.x,
				splite_->vertData[startIndex + 5].pos.y ,
				splite_->vertData[startIndex + 5].pos.z
			};
		}
	}

	*splite_->matrixData = { {
			world *
			view *
		MakeMatrix::PerspectiveFov(
			0.45f,
			static_cast<float>( window_->getWidth() )
			/
			static_cast<float>( window_->getHeight() ),
			0.1f,
			100.0f)},
		{world} // lightのワールド
	};

	ImGui::Begin("Light");
	DirectionalLight light = *splite_->lightData;
	ImGui::DragFloat3("Color", &light.color.x, 0.01f);
	ImGui::DragFloat3("direction", &light.direction.x, 0.01f);
	ImGui::DragFloat("Intensity", &light.intensity, 0.01f);
	light.direction = light.direction.Normalize();
	*splite_->lightData = light;
	ImGui::End();

	TextureManager::SetPSO2CommandList(dxCommon_->getCommandList());
	dxCommon_->getCommandList()->IASetVertexBuffers(0, 1, &splite_->vbView);
	//形状設定.PSOのものとはまた別(同じものを設定する)
	dxCommon_->getCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		0, //RootParameter配列の 0 番目
		splite_->materialBuff->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		1, //RootParameter配列の 1 番目
		splite_->matrixBuff->GetGPUVirtualAddress()
	);
	dxCommon_->getCommandList()->SetGraphicsRootConstantBufferView(
		3, //RootParameter配列の 1 番目
		splite_->lightBuff->GetGPUVirtualAddress()
	);

	ID3D12DescriptorHeap* ppHeaps[] = { dxCommon_->getSrvDescriptorHeap() };
	dxCommon_->getCommandList()->SetDescriptorHeaps(1, ppHeaps);

	dxCommon_->getCommandList()->SetGraphicsRootDescriptorTable(
		2,
		TextureManager::getGpuHandle(textureNum)
	);
	// 描画!!!
	dxCommon_->getCommandList()->DrawInstanced((UINT)( kSubDivision * kSubDivision * 6 ), 1, 0, 0);

}

void System::CreatePrimitivePSO() {
	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	// SRV を扱うように設定
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// offset を自動計算するように 設定
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// CBV を使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// PixelShderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// レジスタ番号0 とバインド
	// register(b0) の 0. b11 なら 11
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// VertexShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	// パラメーターをDESCにセット
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);
	if(FAILED(hr)) {
		Logger::OutputLog(reinterpret_cast<char*>( errorBlob->GetBufferPointer() ));
		assert(false);
	}

	//バイナリをもとに作成
	dxCommon_->getDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&primitivePSO_->rootSignature)
	);
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";/*Semantics*/
	inputElementDescs[0].SemanticIndex = 0;/*Semanticsの横に書いてある数字(今回はPOSITION0なので 0 )*/
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float 4
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "COLOR";/*Semantics*/
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	ShaderCompiler compiler;
	compiler.Init();

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Primitive.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = compiler.CompileShader(L"./Code/System/Shader/Primitive.PS.hlsl", L"ps_6_0");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = primitivePSO_->rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize()
	};

	// DepthStancilState の設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depth の機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual (近ければ描画する)
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencil の設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込む用のRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)タイプ。三角形を設定する
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 生成
	hr = dxCommon_->getDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&primitivePSO_->pipelineState)
	);
	assert(SUCCEEDED(hr));
}

bool System::ProcessMessage() {
	return window_->ProcessMessage();
}

void System::BeginFrame() {
	ImGuiManager::getInstance()->Begin();
	dxCommon_->PreDraw();

	triangle_->index = 0;
}

void System::EndFrame() {
	ImGuiManager::getInstance()->End();
	ImGuiManager::getInstance()->Draw(dxCommon_.get());
	dxCommon_->PostDraw();
}

int System::LoadTexture(const std::string& filePath) {
	return TextureManager::LoadTexture(filePath);
}