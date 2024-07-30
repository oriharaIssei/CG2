#include "SphereObject.h"

#include "PrimitiveDrawer.h"
#include "System.h"

#include <imgui.h>

#include "Vector3.h"

void SphereObject::Init([[maybe_unused]] const std::string &directryPath,const std::string &objectName){
	name_ = objectName;
	materialManager_ = System::getInstance()->getMaterialManager();

	material_ = materialManager_->Create("white");

	transform_.Init();
}

void SphereObject::Updata(){
	ImGui::Text("Name : %s",name_.c_str());
	IGameObject::Updata();
}

void SphereObject::Draw(const ViewProjection &viewProj){
	constexpr int kSubDivision = 16;
	const float kLonEvery = static_cast<float>(M_PI) * 2.0f / static_cast<float>(kSubDivision);
	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubDivision);

	Vector3 vert[4];

	for(int latIndex = 0; latIndex < kSubDivision; ++latIndex){
		float lat = -static_cast<float>(M_PI) / 2.0f + (kLatEvery * latIndex);
		for(int lonIndex = 0; lonIndex < kSubDivision; ++lonIndex){
			float lon = lonIndex * kLonEvery;

			// lb 0,0
			vert[0] = {Vector3(
				std::cosf(lat) * std::cosf(lon),
				std::sinf(lat),
				std::cosf(lat) * std::sinf(lon)
			)
			};

			// lt 0,1
			vert[1] = {Vector3(
				std::cosf(lat + kLatEvery) * std::cosf(lon),
				std::sinf(lat + kLatEvery),
				std::cosf(lat + kLatEvery) * std::sinf(lon)
			)
			};

			//rb 1,0
			vert[2] = {Vector3(
				std::cosf(lat) * std::cosf(lon + kLonEvery),
				std::sinf(lat),
				std::cosf(lat) * std::sinf(lon + kLonEvery)
			)
			};

			// rt 1,1
			vert[3] = {Vector3(
				std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
				std::sinf(lat + kLatEvery),
				std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery)
			)
			};

			PrimitiveDrawer::Quad(vert[0],vert[1],vert[2],vert[3],transform_,viewProj,material_);
		}
	}
}