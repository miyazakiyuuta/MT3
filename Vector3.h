#pragma once

struct Vector3 {
	float x;
	float y;
	float z;
};

namespace Vector3Math {
	// 加算
	Vector3 Add(const Vector3& v1, const Vector3& v2);
	// 減算
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	// スカラー倍
	Vector3 Multiply(float scalar, const Vector3& v);
	// 内積
	float Dot(const Vector3& v1, const Vector3& v2);
	// 長さ(ノルム)
	float Length(const Vector3& v);
	// 正規化
	Vector3 Normalize(const Vector3& v);
	// クロス積
	Vector3 Cross(const Vector3& v1, const Vector3& v2);
}

inline Vector3 operator-(const Vector3& v) { return { -v.x,-v.y,-v.z }; }
inline Vector3 operator+(const Vector3& v) { return v; }

inline Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Vector3Math::Add(v1, v2); }
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Vector3Math::Subtract(v1, v2); }
inline Vector3 operator*(float s, const Vector3& v) { return Vector3Math::Multiply(s, v); }
inline Vector3 operator*(Vector3& v, float s) { return s * v; }
inline Vector3 operator/(const Vector3& v, float s) { return Vector3Math::Multiply(1.0f / s, v); }