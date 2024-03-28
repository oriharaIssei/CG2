#pragma once

#include <memory>
#include <string>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ShaderCompiler.h"
#include "PipelineStateObj.h"
#include "TriangleBufferVerw.h"
#include "Splite.h"

#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix4x4.h>

struct VertexData {
	Vector4 pos;
	Vector2 texCoord;
};

class System {
public:
	static void Init();
	static void Finalize();

	static bool ProcessMessage();

	static void BeginFrame();
	static void EndFrame();

	static int LoadTexture(const std::string& filePath);

	static void DrawTriangle(const Matrix4x4& wvp);
	static void DrawTexture(const Matrix4x4& wvp, int textureNum);
	static void DrawSprite(const Matrix4x4& wvp, int textureNum);
private:
	static std::unique_ptr<WinApp> window_;
	static std::unique_ptr<DirectXCommon> dxCommon_;
	static std::unique_ptr<ShaderCompiler> shaderCompiler_;
	static std::unique_ptr<PipelineStateObj> pso_;

	static std::unique_ptr<TriangleBufferVerw> triangle_;
	static std::unique_ptr<Splite> splite_;
	
	static Microsoft::WRL::ComPtr<ID3D12Resource> constantBuff_ ;
	static Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
public:
	static WinApp* getWinApp(){ return window_.get(); }
	static DirectXCommon* getDxCommon(){ return dxCommon_.get(); }
	static ShaderCompiler* getShaderCompiler(){ return shaderCompiler_.get(); }
	static PipelineStateObj* getPso(){ return pso_.get(); }
};