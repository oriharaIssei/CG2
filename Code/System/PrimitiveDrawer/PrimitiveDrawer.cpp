#include "PrimitiveDrawer.h"

#include <Vector4.h>

WorldTransform PrimitiveDrawer::transform;
ViewProjection PrimitiveDrawer::viewProj;

Material *PrimitiveDrawer::standerdMaterial_ = nullptr;
PipelineStateObj *PrimitiveDrawer::trianglePso_ = nullptr;
std::unique_ptr<PipelineStateObj> PrimitiveDrawer::linePso_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> PrimitiveDrawer::commandList_ = nullptr;

std::unique_ptr<PrimitiveObject3dMesh> PrimitiveDrawer::lineMesh_ = nullptr;
uint32_t PrimitiveDrawer::lineInstanceVal_ = 0;

std::unique_ptr<PrimitiveObject3dMesh> PrimitiveDrawer::triangleMesh_ = nullptr;
uint32_t PrimitiveDrawer::triangleInstanceVal_ = 0;

std::unique_ptr<PrimitiveObject3dMesh>PrimitiveDrawer::quadMesh_ = nullptr;
uint32_t PrimitiveDrawer::quadInstanceVal_ = 0;

void PrimitiveDrawer::Init() {
	trianglePso_ = System::getInstance()->getPrimitivePso();
	CreateLinePso();
	commandList_ = System::getInstance()->getDxCommon()->getCommandList();

	standerdMaterial_ = System::getInstance()->getStanderdMaterial().get();

	lineMesh_ = std::make_unique<PrimitiveObject3dMesh>();
	lineMesh_->Create(2 * 600);
	lineInstanceVal_ = 0;

	triangleMesh_ = std::make_unique<PrimitiveObject3dMesh>();
	triangleMesh_->Create(3 * 400);
	triangleInstanceVal_ = 0;

	quadMesh_ = std::make_unique<PrimitiveObject3dMesh>();
	quadMesh_->Create(6 * 200);
	quadInstanceVal_ = 0;
}

void PrimitiveDrawer::Finalize() {
	transform.Finalize();
	viewProj.Finalize();


	commandList_.Reset();
	linePso_->Finalize();
	linePso_.release();

	lineMesh_->Finalize();
	triangleMesh_->Finalize();
	quadMesh_->Finalize();
}

void PrimitiveDrawer::Line(const Vector3 &p0, const Vector3 &p1, const WorldTransform &transform, const ViewProjection &viewProj) {
	const uint32_t startIndex = lineInstanceVal_ * 3;

	lineMesh_->vertData[startIndex].pos = { p0.x,p0.y,p0.z,1.0f };
	lineMesh_->vertData[startIndex].normal = p0;
	lineMesh_->vertData[startIndex + 1].pos = { p1.x,p1.y,p1.z,1.0f };
	lineMesh_->vertData[startIndex + 1].normal = p1;

	lineMesh_->indexData[startIndex] = startIndex;
	lineMesh_->indexData[startIndex + 1] = startIndex + 1;

	commandList_->SetGraphicsRootSignature(linePso_->rootSignature.Get());
	commandList_->SetPipelineState(linePso_->pipelineState.Get());

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList_->IASetVertexBuffers(0, 1, &lineMesh_->vbView);
	commandList_->IASetIndexBuffer(&lineMesh_->ibView);

	transform.SetForRootParameter(commandList_.Get(), 0);
	viewProj.SetForRootParameter(commandList_.Get(), 1);
	standerdMaterial_->SetForRootParameter(commandList_.Get(), 2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList_.Get(), 3);


	commandList_->DrawIndexedInstanced(
		3, 1, startIndex * 3, 0, 0
	);
	++lineInstanceVal_;
}

void PrimitiveDrawer::Line(const Vector3 &p0, const Vector3 &p1, const WorldTransform &transform, const ViewProjection &viewProj, const Material &material) {
	const uint32_t startIndex = lineInstanceVal_ * 3;
	lineMesh_->vertData[startIndex].pos = { p0.x,p0.y,p0.z,1.0f };
	lineMesh_->vertData[startIndex].normal = p0;
	lineMesh_->vertData[startIndex + 1].pos = { p1.x,p1.y,p1.z,1.0f };
	lineMesh_->vertData[startIndex + 1].normal = p1;

	lineMesh_->indexData[startIndex] = startIndex;
	lineMesh_->indexData[startIndex + 1] = startIndex + 1;

	commandList_->SetGraphicsRootSignature(linePso_->rootSignature.Get());
	commandList_->SetPipelineState(linePso_->pipelineState.Get());

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &lineMesh_->vbView);
	commandList_->IASetIndexBuffer(&lineMesh_->ibView);

	transform.SetForRootParameter(commandList_.Get(), 0);
	viewProj.SetForRootParameter(commandList_.Get(), 1);
	material.SetForRootParameter(commandList_.Get(), 2);

	commandList_->DrawIndexedInstanced(
		2, 1, startIndex * 2, 0, 0
	);
	++lineInstanceVal_;
}

void PrimitiveDrawer::Triangle(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const WorldTransform &transform, const ViewProjection &viewProj) {
	const uint32_t startIndex = triangleInstanceVal_ * 2;
	triangleMesh_->vertData[startIndex].pos = { p0.x,p0.y,p0.z,1.0f };
	triangleMesh_->vertData[startIndex].normal = p0;
	triangleMesh_->vertData[startIndex + 1].pos = { p1.x,p1.y,p1.z,1.0f };
	triangleMesh_->vertData[startIndex + 1].normal = p1;
	triangleMesh_->vertData[startIndex + 1].pos = { p2.x,p2.y,p2.z,1.0f };
	triangleMesh_->vertData[startIndex + 1].normal = p2;

	triangleMesh_->indexData[startIndex] = startIndex;
	triangleMesh_->indexData[startIndex + 1] = startIndex + 1;
	triangleMesh_->indexData[startIndex + 2] = startIndex + 2;

	commandList_->SetGraphicsRootSignature(trianglePso_->rootSignature.Get());
	commandList_->SetPipelineState(trianglePso_->pipelineState.Get());

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &triangleMesh_->vbView);
	commandList_->IASetIndexBuffer(&triangleMesh_->ibView);

	transform.SetForRootParameter(commandList_.Get(), 0);
	viewProj.SetForRootParameter(commandList_.Get(), 1);
	standerdMaterial_->SetForRootParameter(commandList_.Get(), 2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList_.Get(), 3);

	commandList_->DrawIndexedInstanced(
		2, 1, startIndex * 2, 0, 0
	);
	++triangleInstanceVal_;
}

void PrimitiveDrawer::Triangle(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const WorldTransform &transform, const ViewProjection &viewProj, const Material &material) {
	const uint32_t startIndex = triangleInstanceVal_ * 3;
	triangleMesh_->vertData[startIndex].pos = { p0.x,p0.y,p0.z,1.0f };
	triangleMesh_->vertData[startIndex].normal = p0;
	triangleMesh_->vertData[startIndex + 1].pos = { p1.x,p1.y,p1.z,1.0f };
	triangleMesh_->vertData[startIndex + 1].normal = p1;
	triangleMesh_->vertData[startIndex + 2].pos = { p2.x,p2.y,p2.z,1.0f };
	triangleMesh_->vertData[startIndex + 2].normal = p2;

	triangleMesh_->indexData[startIndex] = startIndex;
	triangleMesh_->indexData[startIndex + 1] = startIndex + 1;
	triangleMesh_->indexData[startIndex + 2] = startIndex + 2;

	commandList_->SetGraphicsRootSignature(trianglePso_->rootSignature.Get());
	commandList_->SetPipelineState(trianglePso_->pipelineState.Get());

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &triangleMesh_->vbView);
	commandList_->IASetIndexBuffer(&triangleMesh_->ibView);

	transform.SetForRootParameter(commandList_.Get(), 0);
	viewProj.SetForRootParameter(commandList_.Get(), 1);
	material.SetForRootParameter(commandList_.Get(), 2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList_.Get(), 3);

	commandList_->DrawIndexedInstanced(
		3, 1, startIndex * 3, 0, 0
	);
	++triangleInstanceVal_;
}

void PrimitiveDrawer::Quad(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const WorldTransform &transform, const ViewProjection &viewProj) {
	const uint32_t startIndex = quadInstanceVal_ * 6;
	const uint32_t startVertex = quadInstanceVal_ * 6;
	quadMesh_->vertData[startIndex].pos = { p0.x,p0.y,p0.z,1.0f };
	quadMesh_->vertData[startIndex].normal = p0;
	quadMesh_->vertData[startIndex + 1].pos = { p1.x,p1.y,p1.z,1.0f };
	quadMesh_->vertData[startIndex + 1].normal = p1;
	quadMesh_->vertData[startIndex + 2].pos = { p2.x,p2.y,p2.z,1.0f };
	quadMesh_->vertData[startIndex + 2].normal = p2;
	quadMesh_->vertData[startIndex + 3].pos = { p3.x,p3.y,p3.z,1.0f };
	quadMesh_->vertData[startIndex + 3].normal = p3;

	quadMesh_->indexData[startIndex] = startIndex;
	quadMesh_->indexData[startIndex + 1] = startIndex + 1;
	quadMesh_->indexData[startIndex + 2] = startIndex + 2;
	quadMesh_->indexData[startIndex + 3] = startIndex + 1;
	quadMesh_->indexData[startIndex + 4] = startIndex + 3;
	quadMesh_->indexData[startIndex + 5] = startIndex + 2;

	commandList_->SetGraphicsRootSignature(trianglePso_->rootSignature.Get());
	commandList_->SetPipelineState(trianglePso_->pipelineState.Get());

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &quadMesh_->vbView);
	commandList_->IASetIndexBuffer(&quadMesh_->ibView);

	transform.SetForRootParameter(commandList_.Get(), 0);
	viewProj.SetForRootParameter(commandList_.Get(), 1);
	standerdMaterial_->SetForRootParameter(commandList_.Get(), 2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList_.Get(), 3);

	commandList_->DrawIndexedInstanced(
		6, 1, startIndex, startVertex, 0
	);
	++quadInstanceVal_;
}

void PrimitiveDrawer::Quad(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const WorldTransform &transform, const ViewProjection &viewProj, const Material &material) {
	const uint32_t startIndex = quadInstanceVal_ * 6;
	const uint32_t startVertex = quadInstanceVal_ * 4;
	quadMesh_->vertData[startIndex].pos = { p0.x,p0.y,p0.z,1.0f };
	quadMesh_->vertData[startIndex].normal = p0;
	quadMesh_->vertData[startIndex + 1].pos = { p1.x,p1.y,p1.z,1.0f };
	quadMesh_->vertData[startIndex + 1].normal = p1;
	quadMesh_->vertData[startIndex + 2].pos = { p2.x,p2.y,p2.z,1.0f };
	quadMesh_->vertData[startIndex + 2].normal = p2;
	quadMesh_->vertData[startIndex + 3].pos = { p3.x,p3.y,p3.z,1.0f };
	quadMesh_->vertData[startIndex + 3].normal = p3;

	quadMesh_->indexData[startIndex] = startIndex;
	quadMesh_->indexData[startIndex + 1] = startIndex + 1;
	quadMesh_->indexData[startIndex + 2] = startIndex + 2;
	quadMesh_->indexData[startIndex + 3] = startIndex + 1;
	quadMesh_->indexData[startIndex + 4] = startIndex + 3;
	quadMesh_->indexData[startIndex + 5] = startIndex + 2;

	commandList_->SetGraphicsRootSignature(trianglePso_->rootSignature.Get());
	commandList_->SetPipelineState(trianglePso_->pipelineState.Get());

	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &quadMesh_->vbView);
	commandList_->IASetIndexBuffer(&quadMesh_->ibView);

	transform.SetForRootParameter(commandList_.Get(), 0);
	viewProj.SetForRootParameter(commandList_.Get(), 1);
	material.SetForRootParameter(commandList_.Get(), 2);
	System::getInstance()->getStanderdLight()->SetForRootParameter(commandList_.Get(), 3);

	commandList_->DrawIndexedInstanced(
		6, 1, startIndex, startVertex, 0
	);
	++quadInstanceVal_;
}

void PrimitiveDrawer::CreateLinePso(System *system) {
	linePso_ = std::make_unique<PipelineStateObj>();
	system->CreatePrimitivePSO(linePso_, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}
