#include "PrimitiveDrawer.h"

#include "DXFunctionHelper.h"
#include "System.h"

#include <Vector4.h>

std::unique_ptr<DXCommand> PrimitiveDrawer::dxCommand_;

PipelineStateObj *PrimitiveDrawer::trianglePso_ = nullptr;
std::unique_ptr<PipelineStateObj> PrimitiveDrawer::linePso_ = nullptr;

std::unique_ptr<PrimitiveObject3dMesh> PrimitiveDrawer::lineMesh_ = nullptr;
uint32_t PrimitiveDrawer::lineInstanceVal_ = 0;

std::unique_ptr<PrimitiveObject3dMesh> PrimitiveDrawer::triangleMesh_ = nullptr;
uint32_t PrimitiveDrawer::triangleInstanceVal_ = 0;

std::unique_ptr<PrimitiveObject3dMesh>PrimitiveDrawer::quadMesh_ = nullptr;
uint32_t PrimitiveDrawer::quadInstanceVal_ = 0;

void PrimitiveDrawer::Init() {
	dxCommand_ = std::make_unique<DXCommand>();
	dxCommand_->Init(System::getInstance()->getDXDevice()->getDevice(),"main","main");

	trianglePso_ = System::getInstance()->getPrimitivePso();
	CreateLinePso();

	lineMesh_ = std::make_unique<PrimitiveObject3dMesh>();
	lineMesh_->Create(2 * 600,0);
	lineInstanceVal_ = 0;

	triangleMesh_ = std::make_unique<PrimitiveObject3dMesh>();
	triangleMesh_->Create(3 * 600,0);
	triangleInstanceVal_ = 0;

	quadMesh_ = std::make_unique<PrimitiveObject3dMesh>();
	quadMesh_->Create(4 * 600,6 * 600);
	quadInstanceVal_ = 0;
}

void PrimitiveDrawer::Finalize() {
	dxCommand_->Finalize();

	linePso_->Finalize();
	linePso_.release();

	lineMesh_->Finalize();
	triangleMesh_->Finalize();
	quadMesh_->Finalize();
}

void PrimitiveDrawer::Line(const Vector3 &p0,const Vector3 &p1,const WorldTransform &transform,const ViewProjection &viewProj,const Material *material) {
	ID3D12GraphicsCommandList *commandList = dxCommand_->getCommandList();

	const uint32_t startIndex = lineInstanceVal_ * 3;
	lineMesh_->vertData[startIndex].pos = {p0.x,p0.y,p0.z,1.0f};
	lineMesh_->vertData[startIndex].normal = p0;
	lineMesh_->vertData[startIndex + 1].pos = {p1.x,p1.y,p1.z,1.0f};
	lineMesh_->vertData[startIndex + 1].normal = p1;

	commandList->SetGraphicsRootSignature(linePso_->rootSignature.Get());
	commandList->SetPipelineState(linePso_->pipelineState.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList->IASetVertexBuffers(0,1,&lineMesh_->vbView);
	commandList->IASetIndexBuffer(&lineMesh_->ibView);

	transform.SetForRootParameter(commandList,0);
	viewProj.SetForRootParameter(commandList,1);
	material->SetForRootParameter(commandList,2);

	commandList->DrawInstanced(
		2,1,startIndex * 2,0
	);
	++lineInstanceVal_;
}

void PrimitiveDrawer::Triangle(const Vector3 &p0,const Vector3 &p1,const Vector3 &p2,const WorldTransform &transform,const ViewProjection &viewProj,const Material *material) {
	ID3D12GraphicsCommandList *commandList = dxCommand_->getCommandList();

	const uint32_t startIndex = triangleInstanceVal_ * 3;
	triangleMesh_->vertData[startIndex].pos = {p0.x,p0.y,p0.z,1.0f};
	triangleMesh_->vertData[startIndex].normal = p0;
	triangleMesh_->vertData[startIndex + 1].pos = {p1.x,p1.y,p1.z,1.0f};
	triangleMesh_->vertData[startIndex + 1].normal = p1;
	triangleMesh_->vertData[startIndex + 2].pos = {p2.x,p2.y,p2.z,1.0f};
	triangleMesh_->vertData[startIndex + 2].normal = p2;

	commandList->SetGraphicsRootSignature(trianglePso_->rootSignature.Get());
	commandList->SetPipelineState(trianglePso_->pipelineState.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->IASetVertexBuffers(0,1,&triangleMesh_->vbView);

	transform.SetForRootParameter(commandList,0);
	viewProj.SetForRootParameter(commandList,1);
	material->SetForRootParameter(commandList,2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList,3);

	commandList->DrawInstanced(
		3,1,startIndex * 3,0
	);
	++triangleInstanceVal_;
}

void PrimitiveDrawer::Quad(const Vector3 &p0,const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,const WorldTransform &transform,const ViewProjection &viewProj,const Material *material) {
	ID3D12GraphicsCommandList *commandList = dxCommand_->getCommandList();

	const uint32_t startVertex = quadInstanceVal_ * 4;
	const uint32_t startIndex = quadInstanceVal_ * 6;

	quadMesh_->vertData[startVertex].pos = {p0.x,p0.y,p0.z,1.0f};
	quadMesh_->vertData[startVertex].normal = p0;
	quadMesh_->vertData[startVertex + 1].pos = {p1.x,p1.y,p1.z,1.0f};
	quadMesh_->vertData[startVertex + 1].normal = p1;
	quadMesh_->vertData[startVertex + 2].pos = {p2.x,p2.y,p2.z,1.0f};
	quadMesh_->vertData[startVertex + 2].normal = p2;
	quadMesh_->vertData[startVertex + 3].pos = {p3.x,p3.y,p3.z,1.0f};
	quadMesh_->vertData[startVertex + 3].normal = p3;

	quadMesh_->indexData[startIndex] = startVertex;
	quadMesh_->indexData[startIndex + 1] = startVertex + 1;
	quadMesh_->indexData[startIndex + 2] = startVertex + 2;
	quadMesh_->indexData[startIndex + 3] = startVertex + 1;
	quadMesh_->indexData[startIndex + 4] = startVertex + 3;
	quadMesh_->indexData[startIndex + 5] = startVertex + 2;

	commandList->SetGraphicsRootSignature(trianglePso_->rootSignature.Get());
	commandList->SetPipelineState(trianglePso_->pipelineState.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->IASetVertexBuffers(0,1,&quadMesh_->vbView);
	commandList->IASetIndexBuffer(&quadMesh_->ibView);

	transform.SetForRootParameter(commandList,0);
	viewProj.SetForRootParameter(commandList,1);
	material->SetForRootParameter(commandList,2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList,3);

	commandList->DrawIndexedInstanced(6,1,0,startVertex,0);
	++quadInstanceVal_;
}

void PrimitiveDrawer::CreateLinePso(System *system) {
	linePso_ = std::make_unique<PipelineStateObj>();
	system->CreatePrimitivePSO(linePso_,D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}
