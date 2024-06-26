#include "AllViewState.h"
#include "ChipEditState.h"

#include "MapEditor.h"

#include "imgui.h"

void AllViewState::Update() {
	ImGui::DragFloat2("ChipSize",&MapEditor::EditChip::size.x);

	preMapSize_ = {host_->getChipList()[0].size() - 1,host_->getChipList().size() - 1};
	mapSize_ = preMapSize_;

	ImGui::InputInt("Col",(int *)&mapSize_.second,1);
	ImGui::InputInt("Row",(int *)&mapSize_.first,1);
	mapSize_ = {(std::max<uint32_t>)(mapSize_.first,0),(std::max<uint32_t>)(mapSize_.second,0)};

	if(mapSize_.first != preMapSize_.first) {
		EditRow();
	}
	if(mapSize_.second != preMapSize_.second) {
		EditCol();
	}

	ImGui::InputInt("Focus Col :",(int *)&currentAddress_.second);
	ImGui::InputInt("Focus Row :",(int *)&currentAddress_.first);
	currentAddress_ = {
		(std::clamp<uint32_t>)(currentAddress_.first,0,host_->getChipList()[0].size() - 1),
		(std::clamp<uint32_t>)(currentAddress_.second,0,host_->getChipList().size() - 1)
	};

	if(ImGui::Button("Focus currentAddress")) {
		host_->TransitionState(new ChipEditState(host_,currentAddress_));
	}
}

void AllViewState::Draw(const ViewProjection &viewProj) {
	for(auto &editChipCol : host_->getChipList()) {
		for(auto &editChip : editChipCol) {
			editChip->Draw(viewProj);
		}
	}
}

void AllViewState::EditCol() {
	// Col +
	if(mapSize_.second > preMapSize_.second) {
		for(uint32_t newCol = preMapSize_.second + 1; newCol <= mapSize_.second; ++newCol) {
			host_->PushBackChipList(std::vector<std::unique_ptr<MapEditor::EditChip>>());
			for(uint32_t row = 0; row < host_->getChipList()[0].size(); ++row) {
				host_->PushBackChip(newCol,std::make_unique< MapEditor::EditChip>());
				host_->getChipList()[newCol].back()->Init(host_,{row,newCol});
			}
		}
		// Col -
	} else if(mapSize_.second < preMapSize_.second) {
		for(uint32_t col = preMapSize_.second; col > mapSize_.second; --col) {
			if(col < host_->getChipList().size()) {
				host_->PopBackChipList();
			}
		}
	}
}

void AllViewState::EditRow() {
	// Row +
	if(mapSize_.first > preMapSize_.first) {
		for(uint32_t col = 0; col <= mapSize_.second; ++col) {
			for(uint32_t row = preMapSize_.first + 1; row <= mapSize_.first; ++row) {
				host_->PushBackChip(col,std::make_unique<MapEditor::EditChip>());
				host_->getChipList()[col].back()->Init(host_,{row,col});
			}
		}
		// Row -
	} else if(mapSize_.first < preMapSize_.first) {
		for(uint32_t col = 0; col <= mapSize_.second; ++col) {
			for(uint32_t row = preMapSize_.first; row > mapSize_.first; --row) {
				if(row < host_->getChipList()[col].size()) {
					host_->PopBackChip(col);
				}
			}
		}
	}
}