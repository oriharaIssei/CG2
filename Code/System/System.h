#pragma once

#include <memory>
#include <string>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ShaderCompiler.h"
#include "PipelineStateObj.h"
#include "PrimitiveBuffer.h"
#include "Splite.h"

#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix4x4.h>


class System {
public:
	static void Init();
	static void Finalize();

	static bool ProcessMessage();

	static void BeginFrame();
	static void EndFrame();

	static int LoadTexture(const std::string& filePath);

	static void DrawTriangle(const Vector3& position1, const Vector3& position2, const Vector3& position3, const Vector3& scale,  Vector4 color);
	static void DrawQuad(const Vector3& position1, const Vector3& position2, const Vector3& position3 , const Vector3& position4, const Vector3& scale, Vector4 color);
	static void DrawTexture(const Vector3& position1, const Vector3& position2, const Vector3& position3, const Vector3& scale, const Matrix4x4& wvp, Vector4* color, int textureNum);
	static void DrawSprite(const Vector3& ltPos,const Vector2& size, int textureNum);
	static void DrawSphere(const Matrix4x4& world, const Matrix4x4& view, int textureNum);
private:
	static void CreatePrimitivePSO();
private:
	static std::unique_ptr<WinApp> window_;
	static std::unique_ptr<DirectXCommon> dxCommon_;
	static std::unique_ptr<ShaderCompiler> shaderCompiler_;
	static std::unique_ptr<PipelineStateObj> primitivePSO_;

	static std::unique_ptr<PrimitiveBuffer> triangle_;
	static std::unique_ptr<PrimitiveBuffer> quad_;
	static std::unique_ptr<Splite> splite_;

	static Microsoft::WRL::ComPtr<ID3D12Resource> constantBuff_;
	static Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
public:
	static WinApp* getWinApp() { return window_.get(); }
	static DirectXCommon* getDxCommon() { return dxCommon_.get(); }
	static ShaderCompiler* getShaderCompiler() { return shaderCompiler_.get(); }
	static PipelineStateObj* getPso() { return primitivePSO_.get(); }
};