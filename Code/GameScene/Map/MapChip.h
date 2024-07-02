#pragma once

#include <vector>

#include <memory>

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Material.h"

#include <stdint.h>

#include "GameObject.h"

class MapChip {
public:
	void Init(const uint32_t row,const uint32_t col,MaterialManager *materialManager,const std::string &mapDirectory);
	void Update();
	void Draw(float drawDistance,const Vector3 &playerPos,const ViewProjection &viewProj);
private:
	static Vector2 size_;
private:
	std::vector<std::unique_ptr<GameObject>> gameObjects_;

	WorldTransform transform_;
	std::shared_ptr<Material> material_;

	/// <summary>
	/// first = row ,seconds = col
	/// </summary>
	std::pair<uint32_t,uint32_t> address_;

	bool isLoaded_;
public:
	static void setSize(const Vector2 &size) { size_ = size; }
	static const Vector2 &getSize() { return size_; }

	const std::pair<uint32_t,uint32_t> &getAddress()const { return address_; }

	bool getIsLoaded()const { return isLoaded_; }

	const std::unique_ptr<GameObject> &getGameObject(uint32_t index)const { return gameObjects_[index]; }
	const std::vector<std::unique_ptr<GameObject>> &getGameObjectList()const { return gameObjects_; }
};