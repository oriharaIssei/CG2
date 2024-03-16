#pragma once

#include <memory>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ShaderCompiler.h"
#include "PipelineStateObj.h"
#include "TriangleBufferVerw.h"

#include <Vector3.h>
#include <Matrix4x4.h>

class System {
public:
	static void Init();
	static void Finalize();

	static bool ProcessMessage();

	static void BeginFrame();
	static void EndFrame();

	static void DrawTriangle(const Matrix4x4& wvp);

private:
	static std::unique_ptr<WinApp> window_;
	static std::unique_ptr<DirectXCommon> dxCommon_;
	static std::unique_ptr<ShaderCompiler> shaderCompiler_;
	static std::unique_ptr<PipelineStateObj> pso_;
	static std::unique_ptr<TriangleBufferVerw> triangle_;

	static Microsoft::WRL::ComPtr<ID3D12Resource> constantBuff_ ;
	static Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
public:
	static WinApp* getWinApp(){ return window_.get(); }
	static DirectXCommon* getDxCommon(){ return dxCommon_.get(); }
	static ShaderCompiler* getShaderCompiler(){ return shaderCompiler_.get(); }
	static PipelineStateObj* getPso(){ return pso_.get(); }
};