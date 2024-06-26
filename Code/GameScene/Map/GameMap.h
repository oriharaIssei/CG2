#pragma once

#include <memory>

#include <vector>

#include <string>

#include "ViewProjection.h"

#include "Vector3.h"
#include <stdint.h>

#include "MapChip.h"

class MapEditor;
class GameMap {
	friend class MapEditor;
public:
	void Init(const std::string &directoryPath,Vector3 *playerPos);
	void Update(const Vector3 &playerPos);
	void Draw(const ViewProjection &viewProj);
private:
	void LoadMapFile();
	void SwapActiveChip();

	struct pair_hash {
		template <class T1,class T2>
		std::size_t operator()(const std::pair<T1,T2> &p) const {
			auto hash1 = std::hash<T1> {}(p.first);
			auto hash2 = std::hash<T2> {}(p.second);
			return hash1 ^ hash2;
		}
	};
private:
	std::vector<std::unique_ptr<MapChip>> activeChips_;

	std::pair<uint32_t,uint32_t> playerAddress_;
	std::pair<uint32_t,uint32_t> playerPreAddress_;

	std::pair<uint32_t,uint32_t> maxAddress_;
	uint32_t loadSize_;

	std::string directoryPath_;
};