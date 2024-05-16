#pragma once

#include <Matrix4x4.h>
#include <Vector2.h>

#include "memory"
#include <string>

#include <Object3dMesh.h>
#include <ViewProjection.h>
#include <WorldTransform.h>

class Sprite {
public:
	static Sprite *Create( const Vector2 &size, const std::string &textureFilePath);
	static void PreDraw();
public:
	void Draw(const WorldTransform &world, const ViewProjection &view);
private:
	Vector2 pos_;
	Vector2 size_;

	std::unique_ptr<TextureObject3dMesh> meshBuff_;

	uint32_t th_;
public:

	const Vector2 &getSize()const { return size_; }
	void setSize(const Vector2 &size) { size_ = size; }
};