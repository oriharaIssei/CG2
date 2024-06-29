#include "AllViewState.h"
#include "ChipEditState.h"

#include "MapEditor.h"

#include "imgui.h"

void AllViewState::Update() {
	ImGui::DragFloat2("ChipSize",&MapEditor::EditChip::size.x);

	preMapSize_ = {host_->getChipList()[0].size() - 1,host_->getChipList().size() - 1};
	mapSize_ = preMapSize_;

	ImGui::InputInt("Row",(int *)&mapSize_.first,1);
	ImGui::InputInt("Col",(int *)&mapSize_.second,1);
	mapSize_ = {(std::max<uint32_t>)(mapSize_.first,0),(std::max<uint32_t>)(mapSize_.second,0)};

	if(mapSize_.first != preMapSize_.first) {
		EditRow();
	}
	if(mapSize_.second != preMapSize_.second) {
		EditCol();
	}

	ImGui::InputInt("Focus Row :",(int *)&currentAddress_.first);
	ImGui::InputInt("Focus Col :",(int *)&currentAddress_.second);
	currentAddress_ = {
		(std::clamp<uint32_t>)(currentAddress_.first,0,host_->getChipList().size() - 1),
		(std::clamp<uint32_t>)(currentAddress_.second,0,host_->getChipList()[0].size() - 1)
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

void AllViewState::EditRow() {
	//  +
	if(mapSize_.second > preMapSize_.second) {
		for(uint32_t newRow = preMapSize_.second + 1; newRow <= mapSize_.second; ++newRow) {
			host_->PushBackChipList(std::vector<std::unique_ptr<MapEditor::EditChip>>());
			for(uint32_t col = 0; col < host_->getChipList()[0].size(); ++col) {
				host_->PushBackChip(newRow,std::make_unique< MapEditor::EditChip>());
				host_->getChipList()[newRow].back()->Init(host_,{col,newRow});
			}
		}
		//  -
	} else if(mapSize_.second < preMapSize_.second) {
		for(uint32_t row = preMapSize_.second; row > mapSize_.second; --row) {
			if(row < host_->getChipList().size()) {
				host_->PopBackChipList();
			}
		}
	}
}

void AllViewState::EditCol() {
	if(mapSize_.first > preMapSize_.first) {
		for(uint32_t row = 0; row <= mapSize_.second; ++row) {
			for(uint32_t col = preMapSize_.first + 1; col <= mapSize_.first; ++col) {
				host_->PushBackChip(row,std::make_unique<MapEditor::EditChip>());
				host_->getChipList()[row].back()->Init(host_,{row,col});
			}
		}
		// Row -
	} else if(mapSize_.first < preMapSize_.first) {
		for(uint32_t row = 0; row <= mapSize_.second; ++row) {
			for(uint32_t col = preMapSize_.first; col > mapSize_.first; --col) {
				if(row < host_->getChipList()[row].size()) {
					host_->PopBackChip(row);
				}
			}
		}
	}
}