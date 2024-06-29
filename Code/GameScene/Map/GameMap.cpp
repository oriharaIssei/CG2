#include "GameMap.h"

#include <cmath>

#include <algorithm>
#include <cassert>

#include <unordered_set>

void GameMap::Init(const std::string &directoryPath,const Vector3 &playerPos,float loadDistance) {
	directoryPath_ = directoryPath;
	loadDistance_ = loadDistance;

	materialManager_ = std::make_unique<MaterialManager>("MapMaterials");

	LoadMapFile();
	playerAddress_ = {playerPos.x / MapChip::getSize().x,playerPos.z / MapChip::getSize().y};
	SwapActiveChip(playerPos);
}

void GameMap::Update(const Vector3 &playerPos) {
	materialManager_->DebugUpdate();

	playerPreAddress_ = playerAddress_;
	playerAddress_ = {playerPos.x / MapChip::getSize().x,playerPos.z / MapChip::getSize().y};

	if(playerAddress_ != playerPreAddress_) {
		SwapActiveChip(playerPos);
	}
}

void GameMap::Draw(const ViewProjection &viewProj) {
	for(auto &chip : activeChips_) {
		chip->Draw(viewProj);
	}
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
			std::string line;

			MaterialData data;
			std::string name;
			while(std::getline(mapFile,line)) {
				if(line == "EndMaterialList") {
					break;
				}
				std::istringstream iss(line);
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
				}
			}
		}
	}
}

void GameMap::SwapActiveChip(const Vector3 &playerPos) {
	Vector2 player2dPos = {playerPos.x,playerPos.z};

	std::pair<uint32_t,uint32_t> loadMax = {
		static_cast<uint32_t>(ceil(static_cast<float>(MapChip::getSize().x / loadDistance_))),
		static_cast<uint32_t>(ceil(static_cast<float>(MapChip::getSize().y / loadDistance_)))
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
	for(uint32_t row = loadStart.first; row < loadEnd.first; ++row) {
		for(uint32_t col = 0; col < loadEnd.second; ++col) {
			addAddress.insert({row,col});
		}
	}

	auto it = activeChips_.begin();
	while(it != activeChips_.end()) {
		auto &chipAddress = (*it)->getAddress();
		if(addAddress.find(chipAddress) != addAddress.end()) {
			addAddress.erase(chipAddress);
			++it;
		} else {
			it = activeChips_.erase(it);
		}
	}

	Vector2 p[4];
	for(auto &address : addAddress) {
		p[0] = {static_cast<float>(address.second),static_cast<float>(address.first)};
		p[1] = {static_cast<float>(address.second + MapChip::getSize().x),static_cast<float>(address.first)};
		p[2] = {static_cast<float>(address.second),static_cast<float>(address.first + MapChip::getSize().y)};
		p[3] = {static_cast<float>(address.second + MapChip::getSize().x),static_cast<float>(address.first + MapChip::getSize().y)};

		for(size_t i = 0; i < 4; i++) {
			if((player2dPos - p[i]).Length() < loadDistance_ * loadDistance_) {
				auto newChip = std::make_unique<MapChip>();
				newChip->Init(address.first,address.second,materialManager_.get(),directoryPath_);
				activeChips_.push_back(std::move(newChip));
				break;
			}
		}
	}
}