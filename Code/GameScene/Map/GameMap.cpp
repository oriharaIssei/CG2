#include "GameMap.h"

#include <cmath>
#include "Matrix3x3.h"

#include <algorithm>
#include <cassert>

#include <unordered_set>

#include <imgui.h>
#include "System.h"

void GameMap::Init(const std::string &directoryPath,const Vector3 &playerPos,float loadDistance) {
	directoryPath_ = directoryPath;

	loadDistance_ = loadDistance;
	drawDistance_ = 10.0f;

	materialManager_ = std::make_unique<MaterialManager>("MapMaterials");

	LoadMapFile();
	playerAddress_ = {(playerPos.z) / MapChip::getSize().x,playerPos.x / MapChip::getSize().y};

	activeChips_.push_back(std::make_unique<MapChip>());
	activeChips_.back()->Init(playerAddress_.first,playerAddress_.second,materialManager_.get(),directoryPath);
	SwapActiveChip(playerPos);
}

void GameMap::Update(const Vector3 &playerPos) {
	materialManager_->DebugUpdate();

	playerPreAddress_ = playerAddress_;
	playerAddress_ = {(playerPos.z) / MapChip::getSize().x,playerPos.x / MapChip::getSize().y};

	if(playerAddress_ != playerPreAddress_) {
		SwapActiveChip(playerPos);
	}

#ifdef _DEBUG
	ImGui::Begin("GameMap");
	const float preLoadingDistance = loadDistance_;
	ImGui::DragFloat("DrawDistance",&drawDistance_,0.1f,0.1f);

	ImGui::DragFloat("Loading Distance",&loadDistance_,0.3f,drawDistance_);

	if(loadDistance_ != preLoadingDistance) {
		SwapActiveChip(playerPos);
	}
	ImGui::End();

	ImGui::Begin("Address Table");
	ImGui::Text("Player Address %d / %d",playerAddress_.first,playerAddress_.second);
	int numColumns = ceil(static_cast<float>((loadDistance_ + MapChip::getSize().x * 2.0f) / MapChip::getSize().x));
	ImGui::Columns(numColumns);
	ImGui::Text("%d / %d",activeChipAddress_[0].first,activeChipAddress_[0].second);
	ImGui::NextColumn();
	for(int32_t i = 1; i < activeChipAddress_.size(); i++) {
		if(activeChipAddress_[i].first > activeChipAddress_[i - 1].first) {
			ImGui::Columns(numColumns);
		}
		ImGui::Text("%d / %d",activeChipAddress_[i].first,activeChipAddress_[i].second);
		ImGui::NextColumn();
	}
	ImGui::End();

#endif // _DEBUG
}

void GameMap::Draw(const Vector3 &playerPos,const ViewProjection &viewProj) {
	/*std::vector<std::pair<uint32_t,uint32_t>> drawAddress;

	Vector2 player2dNormal = {0.0f,1.0f};
	player2dNormal = TransformVector(player2dNormal,MakeMatrix3::Rotate(viewProj.rotate.y));

	Vector2 player2dPos = {playerPos.x,playerPos.z};

	Vector2 toPlV;
	Vector2 chipVert[4];
	std::pair<uint32_t,uint32_t> address;*/

	/*int32_t drawCount = 0;*/
	for(auto &chip : activeChips_) {
		chip->Draw(drawDistance_,playerPos,viewProj);
	}
	/*address = chip->getAddress();

	chipVert[0] = {static_cast<float>(address.second * MapChip::getSize().x),static_cast<float>(address.first * MapChip::getSize().y)};
	chipVert[1] = chipVert[0] + Vector2(MapChip::getSize().x - 1.0f,0.0f);
	chipVert[2] = chipVert[0] + Vector2(0.0f,MapChip::getSize().y - 1.0f);
	chipVert[3] = chipVert[0] + Vector2(MapChip::getSize().x - 1.0f,MapChip::getSize().y - 1.0f);

	for(int i = 0; i < 4; i++) {
		toPlV = player2dPos - chipVert[i];
		if(player2dNormal.Dot(Normalize(toPlV)) > -0.01f) {
			++drawCount;
			chip->Draw(drawDistance_,playerPos,viewProj);
			drawAddress.push_back(address);
			break;
		}
	}
}
if(drawAddress.empty()) {
	return;
}
std::sort(drawAddress.begin(),drawAddress.end());
int numColumns = ceil(static_cast<float>((loadDistance_ + MapChip::getSize().x * 2.0f) / MapChip::getSize().x));

ImGui::Begin("Draw Address Table");
ImGui::Text("current Draw Count : %d",drawCount);
ImGui::Columns(numColumns);
ImGui::Text("%d / %d",drawAddress[0].second - drawAddress[0].second);
ImGui::NextColumn();
for(int32_t i = 1; i < drawAddress.size(); i++) {
	if(drawAddress[i].first > drawAddress[i - 1].first) {
		ImGui::Columns(numColumns);
	}
	ImGui::Text("%d / %d",drawAddress[i].first,drawAddress[i].second);
	ImGui::NextColumn();
}
ImGui::End();*/

}

void GameMap::LoadMapFile() {
	std::ifstream mapFile(directoryPath_ + '/' + "mapInformation.gmp"); // ファイルを作成/開く

	std::string line;
	assert(mapFile.is_open());
	while(std::getline(mapFile,line)) {
		std::istringstream s(line);
		std::string identifier;
		s >> identifier;
		if(identifier == "ChipSize") {
			Vector2 size;
			s >> size.x >> size.y;
			MapChip::setSize(size);
		} else if(identifier == "MaxAddress") {
			s >> maxAddress_.first >> maxAddress_.second;
			maxAddress_.first -= 1;
			maxAddress_.second -= 1;
		} else if(identifier == "MaterialList") {
			std::string listLine;
			MaterialData data;
			std::string name;
			while(std::getline(mapFile,listLine)) {
				if(listLine == "EndMaterialList") {
					if(materialManager_->getMateriaList().count(name) != 0) {
						data.uvTransform = MakeMatrix::Identity();
						materialManager_->Create(name,data);
					}
					break;
				}
				std::istringstream iss(listLine);
				std::string key;
				iss >> key;

				if(key == "Name") {
					iss >> name;
					continue;
				} else if(key == "Color") {
					iss >> data.color.x >> data.color.y >> data.color.z >> data.color.w;
					continue;
				} else if(key == "EnableLighting") {
					iss >> data.enableLighting;

					data.uvTransform = MakeMatrix::Identity();
					materialManager_->Create(name,data);
					data = {};
				}
			}
		}
	}
}

void GameMap::SwapActiveChip(const Vector3 &playerPos) {
	Vector2 player2dPos = {playerPos.x,playerPos.z};

#ifdef _DEBUG
	if(!activeChipAddress_.empty()) {
		activeChipAddress_.clear();
	}
#endif // _DEBUG

	std::pair<uint32_t,uint32_t> loadMax = {
		static_cast<uint32_t>(ceil(loadDistance_ / static_cast<float>(MapChip::getSize().x))),
		static_cast<uint32_t>(ceil(loadDistance_ / static_cast<float>(MapChip::getSize().y)))
	};

	std::pair<uint32_t,uint32_t> loadStart = {
		static_cast<uint32_t>((std::clamp<int>)(playerAddress_.first - loadMax.first,0,maxAddress_.first)),
		static_cast<uint32_t>((std::clamp<int>)(playerAddress_.second - loadMax.second,0,maxAddress_.second))
	};

	std::pair<uint32_t,uint32_t> loadEnd = {
		static_cast<uint32_t>((std::clamp<int>)(playerAddress_.first + loadMax.first,0,maxAddress_.first)),
		static_cast<uint32_t>((std::clamp<int>)(playerAddress_.second + loadMax.second,0,maxAddress_.second))
	};

	std::unordered_set<std::pair<uint32_t,uint32_t>,pair_hash> addAddress;
	for(uint32_t row = loadStart.first; row <= loadEnd.first; ++row) {
		for(uint32_t col = loadStart.second; col <= loadEnd.second; ++col) {
			addAddress.insert({row,col});
		}
	}

	auto it = activeChips_.begin();
	while(it != activeChips_.end()) {
		auto &chipAddress = (*it)->getAddress();
		if(addAddress.find(chipAddress) != addAddress.end()) {
			addAddress.erase(chipAddress);
			activeChipAddress_.push_back(chipAddress);
			++it;
		} else {
			it = activeChips_.erase(it);
		}
	}

	Vector2 p[4];
	for(auto &address : addAddress) {
		p[0] = {static_cast<float>(address.second * MapChip::getSize().x),static_cast<float>((address.first - 1) * MapChip::getSize().y)};
		p[1] = p[0] + Vector2(MapChip::getSize().x - 1.0f,0.0f);
		p[2] = p[0] + Vector2(0.0f,MapChip::getSize().y - 1.0f);
		p[3] = p[0] + Vector2(MapChip::getSize().x - 1.0f,MapChip::getSize().y - 1.0f);

		for(size_t i = 0; i < 4; i++) {
			if((player2dPos - p[i]).Length() <= loadDistance_ + MapChip::getSize().x) {
				auto newChip = std::make_unique<MapChip>();
				newChip->Init(address.first,address.second,materialManager_.get(),directoryPath_);
				activeChips_.push_back(std::move(newChip));
#ifdef _DEBUG
				activeChipAddress_.push_back(address);
#endif // _DEBUG
				break;
			}
		}
	}
#ifdef _DEBUG
	std::sort(activeChipAddress_.begin(),activeChipAddress_.end());
#endif // _DEBUG
}