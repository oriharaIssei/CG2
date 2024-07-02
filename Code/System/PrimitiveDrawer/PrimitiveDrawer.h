#pragma once

#include <System.h>

#include <Material.h>
#include <Object3dMesh.h>
#include <PipelineStateObj.h>
#include <ViewProjection.h>
#include <WorldTransform.h>

#include "DXCommand.h"

#include <memory>
#include <wrl.h>

#include <stdint.h>
#include <Vector3.h>

class PrimitiveDrawer {
public:
	static void Init();
	static void Finalize();

	static void Line(const Vector3 &p0,const Vector3 &p1,const WorldTransform &transform,const ViewProjection &viewProj,const Material *material);
	static void Triangle(const Vector3 &p0,const Vector3 &p1,const Vector3 &p2,const WorldTransform &transform,const ViewProjection &viewProj,const Material *material);
	static void Quad(const Vector3 &p0,const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,const WorldTransform &transform,const ViewProjection &viewProj,const Material *material);
	
	static void ResetInstanceVal() {
		lineInstanceVal_ = 0; triangleInstanceVal_ = 0; quadInstanceVal_ = 0;
	};
private:
	static void CreateLinePso(System *system = System::getInstance());
private:
	static std::unique_ptr<DXCommand> dxCommand_;

	static PipelineStateObj *trianglePso_;
	static std::unique_ptr<PipelineStateObj> linePso_;

	static std::unique_ptr<PrimitiveObject3dMesh> lineMesh_;
	static uint32_t lineInstanceVal_;

	static std::unique_ptr<PrimitiveObject3dMesh> triangleMesh_;
	static uint32_t triangleInstanceVal_;

	static std::unique_ptr<PrimitiveObject3dMesh> quadMesh_;
	static uint32_t quadInstanceVal_;
};