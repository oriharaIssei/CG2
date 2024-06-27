#include "GameMap.h"

#include <algorithm>
#include <cassert>

#include <unordered_set>

void GameMap::Init(const std::string &directoryPath,Vector3 *playerPos) {
	directoryPath_ = directoryPath;
	LoadMapFile();
}

void GameMap::Update(const Vector3 &playerPos) {
	playerPreAddress_ = playerAddress_;
	playerAddress_ = {playerPos.x / MapChip::getSize().x,playerPos.z / MapChip::getSize().y};

	if(playerAddress_ == playerPreAddress_) {
		SwapActiveChip();
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
	while(std::getline(mapFile,line,',')) {
		std::istringstream s(line);
		std::string identifier;
		s >> identifier;
		if(identifier == "ChipSize") {
			Vector2 size;
			s >> size.x >> size.y;
			MapChip::setSize(size);
		} else if(identifier == "MaxAddress") {
			s >> maxAddress_.first >> maxAddress_.second;
		} else if(identifier == "MaterialList") {
			std::string materialName;
			MaterialData materialData;
			bool isFirst = true;
			while(std::getline(mapFile,line,',')) {
				std::istringstream tokenStream(line);
				std::string token;
				std::getline(tokenStream,token,',');
				if(token == "Name") {
					if(isFirst) {
						isFirst = false;
						tokenStream >> materialName;
						continue;
					}
					materialManager_->Create(materialName,materialData);
					tokenStream >> materialName;
					materialData = MaterialData();
				} else if(token == "Color") {
					tokenStream >> materialData.color.x >> materialData.color.y >> materialData.color.z >> materialData.color.w;
				} else if(token == "EnableLighting") {
					tokenStream >> materialData.enableLighting;
				} else if(token == "EndMaterialList") {
					break;
				}
			}
		}
	}
}

void GameMap::SwapActiveChip() {
	uint32_t startRow = std::clamp<uint32_t>(playerAddress_.first - loadSize_,0,maxAddress_.first);
	uint32_t endRow = std::clamp<uint32_t>(playerAddress_.first + loadSize_,0,maxAddress_.first);

	uint32_t startCol = std::clamp<uint32_t>(playerAddress_.second - loadSize_,0,maxAddress_.second);
	uint32_t endCol = std::clamp<uint32_t>(playerAddress_.second + loadSize_,0,maxAddress_.second);

	std::unordered_set<std::pair<uint32_t,uint32_t>,pair_hash> addAddress;
	for(uint32_t col = startCol; col < endCol; ++col) {
		for(uint32_t row = startRow; row < endRow; row++) {
			addAddress.insert({row,col});
		}
	}

	auto it = activeChips_.begin();
	while(it != activeChips_.end()) {
		auto chipAddress = (*it)->getAddress();
		if(addAddress.find(chipAddress) != addAddress.end()) {
			addAddress.erase(chipAddress);
			++it;
		} else {
			it = activeChips_.erase(it);
		}
	}

	// 必要なアドレスのチップを追加
	for(const auto &address : addAddress) {
		auto newChip = std::make_unique<MapChip>();
		newChip->Init(address.first,address.second,directoryPath_);
		activeChips_.push_back(std::move(newChip));
	}
}