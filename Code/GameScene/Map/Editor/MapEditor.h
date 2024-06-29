#pragma once

#include <vector>

#include <memory>

#include "Material.h"
#include "ViewProjection.h"

#include "Vector2.h"

#include <tchar.h>

#include "GameObject.h"
#include "MapChip.h"

#include "IMapEditState.h"

class MapEditor {
public:
	void Init();
	void Update();
	void Draw(const ViewProjection &viewProj);
public:
	struct EditChip {
		static Vector2 size;

		void Init(MapEditor *host,std::pair<uint32_t,uint32_t> address);
		void Draw(const ViewProjection &viewProj);

		MapEditor *host_;

		std::vector<std::unique_ptr<GameObject>> gameObjects_;
		WorldTransform transform_;
		std::shared_ptr<Material> floorMaterial_;
		/// <summary>
		/// first = row ,seconds = col
		/// </summary>
		std::pair<uint32_t,uint32_t> address_;
	};
private:
	void WriteGameObjectInfo(std::ofstream &mapChipFile,GameObject &mapObj);
	void SaveMapChips();
	void SaveMapInformation();
	void SaveToFile();

	void LoadMaterialList(std::ifstream &mapFile);
	void LoadMapInformation();
	void LoadGameObjectInfo(std::ifstream &mapChipFile,size_t col,size_t row);
	void LoadMapChips();
	void Load();

	std::vector<std::vector<std::unique_ptr<EditChip>>> chips_;

	std::unique_ptr<MaterialManager> materialManager_;

	std::unique_ptr<IMapEditState> currentState_;
public:
	MaterialManager *getMaterialManager()const { return materialManager_.get(); }

	void TransitionState(IMapEditState *nextState);

	const std::vector<std::vector<std::unique_ptr<EditChip>>> &getChipList() const {
		return chips_;
	}

	void PushBackChipList(std::vector<std::unique_ptr<EditChip>> newRow) {
		chips_.push_back(std::move(newRow));
	}

	void PushBackChip(UINT row,std::unique_ptr<EditChip> chip) {
		if(row >= chips_.size()) {
			chips_.resize(row + 1);
		}
		chips_[row].push_back(std::move(chip));
	}

	void PopBackChipList() {
		if(!chips_.empty()) {
			chips_.back().clear();
			chips_.pop_back();
		}
	}

	void PopBackChip(UINT row) {
		if(row < chips_.size() && !chips_[row].empty()) {
			chips_[row].back().reset();
			chips_[row].pop_back();
		}
	}
};