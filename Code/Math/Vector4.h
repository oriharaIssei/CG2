#pragma once

#include <cmath>

struct Vector4 {
	float x;
	float y;
	float z;
	float w;

	// 加算演算子のオーバーロード
	Vector4 operator+(const Vector4 &other) {
		return { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	// 減算演算子のオーバーロード
	Vector4 operator-(const Vector4 &other) {
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	// 乗算演算子のオーバーロード
	Vector4 operator*(float scalar) {
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	// 比較演算子のオーバーロード (==)
	bool operator==(const Vector4 &other) {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	Vector4 cross(const Vector4 &other) {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x,
			0.0f
		};
	}

	// dot積を計算する関数
	float dot(const Vector4 &other) {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	// 長さを計算する関数
	float length() {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}
};