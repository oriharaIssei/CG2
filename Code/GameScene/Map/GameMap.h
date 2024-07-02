#pragma once

#include <memory>

#include <vector>

#include <string>

#include "Material.h"
#include "ViewProjection.h"

#include "Vector3.h"
#include <stdint.h>

#include "MapChip.h"

class GameMap {
public:
	void Init(const std::string &directoryPath,const Vector3 &playerPos,float loadDistance);
	void Update(const Vector3 &playerPos);
	void Draw(const Vector3 &playerPos,const ViewProjection &viewProj);
private:
	void LoadMapFile();
	void SwapActiveChip(const Vector3 &playerPos);

	struct pair_hash {
		template <class T1,class T2>
		std::size_t operator()(const std::pair<T1,T2> &p) const {
			auto hash1 = std::hash<T1> {}(p.first);
			auto hash2 = std::hash<T2> {}(p.second);
			return hash1 ^ hash2;
		}
	};
private:
	float loadDistance_;
	float drawDistance_;

	std::vector<std::unique_ptr<MapChip>> activeChips_;
#ifdef _DEBUG
	std::vector<std::pair<uint32_t,uint32_t>> activeChipAddress_;
#endif // _DEBUG

	std::unique_ptr<MaterialManager> materialManager_;

	std::pair<uint32_t,uint32_t> playerAddress_;
	std::pair<uint32_t,uint32_t> playerPreAddress_;

	std::pair<uint32_t,uint32_t> maxAddress_;

	WorldTransform worldTransform_;

	std::string directoryPath_;
};