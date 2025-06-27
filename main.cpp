#include <Novice.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include <cmath>
#include <imgui.h>
#include <algorithm>

const char kWindowTitle[] = "LE2B_24_ミヤザキ_ユウタ_タイトル";
const int kWindowWidth = 1280; // 画面の横幅
const int kWindowHeight = 720; // 画面の縦幅

const float pi = 3.14159265f;

using namespace MatrixMath;
using namespace Vector3Math;

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Plane {
	Vector3 normal; //!< 法線
	float distance; //!< 距離
};

struct Triangle {
	Vector3 vertices[3]; //!< 頂点
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
	uint32_t color;
};

struct OBB {
	Vector3 center; //!< 中心点
	Vector3 orientations[3]; //!< 座標軸。正規化・直交必須
	Vector3 size; //!< 座標軸方向の長さの半分。中心から面までの距離
};

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char type[]);

Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
Vector3 Perpendicular(const Vector3& vector);

bool IsCollision(const Sphere& s1, const Sphere& s2);
bool IsCollision(const Sphere& sphere, const Plane& plane);
bool IsCollision(const Segment& segment, const Plane& plane);
bool IsCollision(const Triangle& triangle, const Segment& segment);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
bool IsCollision(const AABB& aabb, const Sphere& sphere);
bool IsCollision(const AABB& aabb, const Segment& segment);
bool IsCollision(const OBB& obb, const Sphere& sphere);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, unsigned int color);
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 a{ 0.2f,1.0f,0.0f };
	Vector3 b{ 2.4f,3.1f,1.2f };
	Vector3 c = a + b;
	Vector3 d = a - b;
	Vector3 e = a * 2.4f;
	Vector3 rotate{ 0.4f,1.43f,-0.8f };
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("Window");
		ImGui::Text("c:%f,%f,%f", c.x, c.y, c.z);
		ImGui::Text("d:%f,%f,%f", d.x, d.y, d.z);
		ImGui::Text("e:%f,%f,%f", e.x, e.y, e.z);
		ImGui::Text(
			"matrix:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",
			rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3],
			rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3],
			rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3],
			rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///



		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char type[]) {
	Novice::ScreenPrintf(x, y, "%s", type);
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result =
		Multiply(Dot(v1, v2) * (1.0f / (v2.x * v2.x + v2.y * v2.y + v2.z * v2.z)), v2);
	return result;
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result;
	Vector3 project = Project(
		{ point.x - segment.origin.x,point.y - segment.origin.y,point.z - segment.origin.z }, segment.diff);
	result.x = segment.origin.x + project.x;
	result.y = segment.origin.y + project.y;
	result.z = segment.origin.z + project.z;
	return result;
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	if (Length({ s1.center.x - s2.center.x,s1.center.y - s2.center.y ,s1.center.z - s2.center.z })
		< s1.radius + s2.radius) {
		return true;
	}
	return false;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	// 球の中心位置と平面の法線ベクトルとの内積
	float d = sphere.center.x * plane.normal.x +
		sphere.center.y * plane.normal.y +
		sphere.center.z * plane.normal.z;

	// 球の中心から平面までの距離（符号付き）
	float distance = fabs(d - plane.distance);

	if (distance < sphere.radius) {
		return true;
	}
	return false;
}

bool IsCollision(const Segment& segment, const Plane& plane) {
	float dot = Dot(plane.normal, segment.diff);

	// 垂直=平行であるので、衝突していない
	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / (Dot(segment.diff, plane.normal));

	if (t <= 1.0f && t >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Segment& segment) {
	// 三角形の法線
	Vector3 normal = Cross(
		Subtract(triangle.vertices[1], triangle.vertices[0]),
		Subtract(triangle.vertices[2], triangle.vertices[0]));
	float dot = Dot(normal, segment.diff);
	if (dot == 0.0f) {
		return false;
	}
	float t = Dot(Subtract(triangle.vertices[0], segment.origin), normal) / dot;
	if (t < 0.0f || t>1.0f) {
		return false;
	}

	Vector3 p = Add(segment.origin, Multiply(t, segment.diff));

	Vector3 cross01 = Cross(
		Subtract(triangle.vertices[0], triangle.vertices[1]),
		Subtract(triangle.vertices[1], p));
	Vector3 cross12 = Cross(
		Subtract(triangle.vertices[1], triangle.vertices[2]),
		Subtract(triangle.vertices[2], p));
	Vector3 cross20 = Cross(
		Subtract(triangle.vertices[2], triangle.vertices[0]),
		Subtract(triangle.vertices[0], p));

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && // x軸
		aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y && // y軸
		aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) { // z軸
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	// 最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z),
	};
	// 最近接点と球の中心との距離を求める
	float distance = Length(Subtract(closestPoint, sphere.center));
	// 距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Segment& segment) {

	float txmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float txmax = (aabb.max.x - segment.origin.x) / segment.diff.x;

	float tymin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tymax = (aabb.max.y - segment.origin.y) / segment.diff.y;

	float tzmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tzmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = min(txmin, txmax);
	float tFarX = max(txmin, txmax);
	float tNearY = min(tymin, tymax);
	float tFarY = max(tymin, tymax);
	float tNearZ = min(tzmin, tzmax);
	float tFarZ = max(tzmin, tzmax);

	// AABBとの衝突点(貫通点)のtが小さい方
	float tmin = max(max(tNearX, tNearY), tNearZ);
	// AABBとの衝突点(貫通点)のtが大きい方
	float tmax = min(min(tFarX, tFarY), tFarZ);
	if (tmin <= tmax) {
		if (tmin >= 0.0f && tmax <= 1.0f) {
			return true;
		}
	}
	return false;
}

bool IsCollision(const OBB& obb, const Sphere& sphere) {
	obb;
	sphere;
	return false;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f; // Gridの半分の幅
	const int kSubdivision = 10; // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ
	// 奥から手前への線を順々に引いていく
	for (int xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		// 上の情報を使ってワールド座標系上の始点と終点を求める
		// スクリーン座標系まで変換をかける
		// 変換した座標を使って表示。色は薄い灰色(0xAAAAAAFF)、原点は黒ぐらいが良いが、何でも良い

		float x = -kGridHalfWidth + xIndex * kGridEvery;

		Vector3 start = { x,0.0f,-kGridHalfWidth };
		Vector3 end = { x,0.0f,kGridHalfWidth };

		Vector3 startClip = Transform(start, viewProjectionMatrix);
		Vector3 endClip = Transform(end, viewProjectionMatrix);

		Vector3 startScreen = Transform(startClip, viewportMatrix);
		Vector3 endScreen = Transform(endClip, viewportMatrix);

		unsigned int color;
		if (xIndex == kSubdivision / 2) {
			color = 0x000000FF;
		} else {
			color = 0xAAAAAAFF;
		}

		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}
	// 左から右も同じように徐々に引いていく
	for (int zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + zIndex * kGridEvery;
		Vector3 start = { -kGridHalfWidth,0.0f,z };
		Vector3 end = { kGridHalfWidth,0.0f,z };

		Vector3 startClip = Transform(start, viewProjectionMatrix);
		Vector3 endClip = Transform(end, viewProjectionMatrix);

		Vector3 startScreen = Transform(startClip, viewportMatrix);
		Vector3 endScreen = Transform(endClip, viewportMatrix);

		unsigned int color;
		if (zIndex == kSubdivision / 2) {
			color = 0x000000FF;
		} else {
			color = 0xAAAAAAFF;
		}

		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, unsigned int color) {
	const unsigned int kSubdivision = 10; // 分割数
	const float kLonEvery = 2.0f * pi / float(kSubdivision); // 経度分割1つ分の角度
	const float kLatEvery = pi / float(kSubdivision); // 緯度分割1つ分の角度
	// 緯度の方向に分割 -π/2~π/2
	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		//float lat = -pi / 2.0f + kLatEvery * latIndex; // 現在の緯度
		float lat = -pi / 2.0f + kLatEvery * latIndex; // 現在の緯度
		// 緯度の方向に分割 0 ~ 2π
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = kLonEvery * lonIndex; // 現在の経度
			// world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = {
				sphere.center.x + (std::cos(lat) * std::cos(lon) * sphere.radius),
				sphere.center.y + (std::sin(lat) * sphere.radius),
				sphere.center.z + (std::cos(lat) * std::sin(lon) * sphere.radius)
			};

			b = {
					sphere.center.x + (std::cos(lat + kLatEvery) * std::cos(lon) * sphere.radius),
					sphere.center.y + (std::sin(lat + kLatEvery) * sphere.radius),
					sphere.center.z + (std::cos(lat + kLatEvery) * std::sin(lon) * sphere.radius)
			};

			c = {
				sphere.center.x + (std::cos(lat) * std::cos(lon + kLonEvery) * sphere.radius),
				sphere.center.y + (std::sin(lat) * sphere.radius),
				sphere.center.z + (std::cos(lat) * std::sin(lon + kLonEvery) * sphere.radius)
			};

			// a,b,cをScreen座標系まで変換

			Vector3 aClip = Transform(a, viewProjectionMatrix);
			Vector3 bClip = Transform(b, viewProjectionMatrix);
			Vector3 cClip = Transform(c, viewProjectionMatrix);

			Vector3 aScreen = Transform(aClip, viewportMatrix);
			Vector3 bScreen = Transform(bClip, viewportMatrix);
			Vector3 cScreen = Transform(cClip, viewportMatrix);

			// ab,bcで線を引く
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color);

		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.distance, plane.normal); // 1.中心点を決める
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal)); // 2.法線と垂直なベクトル
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z }; // 3.2の逆ベクトル
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]); // 4.2と法線とのクロス積を求める
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y, -perpendiculars[2].z }; // 5.4の逆ベクトル
	// 6
	Vector3 points[4];
	for (int index = 0; index < 4; index++) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Triangle screenTriangle;
	screenTriangle.vertices[0] = Transform(Transform(triangle.vertices[0], viewProjectionMatrix), viewportMatrix);
	screenTriangle.vertices[1] = Transform(Transform(triangle.vertices[1], viewProjectionMatrix), viewportMatrix);
	screenTriangle.vertices[2] = Transform(Transform(triangle.vertices[2], viewProjectionMatrix), viewportMatrix);
	for (int i = 0; i < 3; i++) {
		Novice::DrawLine(
			int(screenTriangle.vertices[i % 3].x), int(screenTriangle.vertices[i % 3].y),
			int(screenTriangle.vertices[(i + 1) % 3].x), int(screenTriangle.vertices[(i + 1) % 3].y), color);
	}
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertexes[8];
	vertexes[0] = aabb.min;
	vertexes[1] = { aabb.max.x,aabb.min.y,aabb.min.z };
	vertexes[2] = { aabb.max.x,aabb.min.y,aabb.max.z };
	vertexes[3] = { aabb.min.x,aabb.min.y,aabb.max.z };
	vertexes[4] = { aabb.min.x,aabb.max.y,aabb.min.z };
	vertexes[5] = { aabb.max.x,aabb.max.y,aabb.min.z };
	vertexes[6] = aabb.max;
	vertexes[7] = { aabb.min.x,aabb.max.y,aabb.max.z };
	Vector3 screenVertexes[8];
	for (int i = 0; i < 8; i++) {
		screenVertexes[i] = Transform(Transform(vertexes[i], viewProjectionMatrix), viewportMatrix);
	}
	for (int i = 0; i < 4; i++) {
		Novice::DrawLine(
			int(screenVertexes[i].x), int(screenVertexes[i].y),
			int(screenVertexes[(i + 1) % 4].x), int(screenVertexes[(i + 1) % 4].y),
			color);
		Novice::DrawLine(
			int(screenVertexes[i + 4].x), int(screenVertexes[i + 4].y),
			int(screenVertexes[(i + 1) % 4 + 4].x), int(screenVertexes[(i + 1) % 4 + 4].y),
			color);
		Novice::DrawLine(
			int(screenVertexes[i].x), int(screenVertexes[i].y),
			int(screenVertexes[i + 4].x), int(screenVertexes[i + 4].y),
			color);
	}
}

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 xAxis = Multiply(obb.size.x, obb.orientations[0]);
	Vector3 yAxis = Multiply(obb.size.y, obb.orientations[1]);
	Vector3 zAxis = Multiply(obb.size.z, obb.orientations[2]);
	Vector3 vertexes[8];
	vertexes[0] = Add(obb.center, Add(xAxis, Add(yAxis, zAxis)));
	vertexes[1] = Add(obb.center, Add(xAxis, Subtract(yAxis, zAxis)));
	vertexes[2] = Add(obb.center, Subtract(xAxis, Add(yAxis, zAxis)));
	vertexes[3] = Add(obb.center, Subtract(xAxis, Subtract(yAxis, zAxis)));
	vertexes[4] = Subtract(obb.center, Add(xAxis, Add(yAxis, zAxis)));
	vertexes[5] = Subtract(obb.center, Subtract(xAxis, Add(yAxis, zAxis)));
	vertexes[6] = Subtract(obb.center, Add(xAxis, Subtract(yAxis, zAxis)));
	vertexes[7] = Subtract(obb.center, Subtract(xAxis, Subtract(yAxis, zAxis)));

	vertexes[0] = Subtract(obb.center, Add(xAxis, Subtract(yAxis, zAxis)));
	vertexes[1] = Subtract(obb.center, Add(xAxis, Add(yAxis, zAxis)));
	vertexes[2] = Add(obb.center, Add(xAxis, Add(yAxis, zAxis)));
	vertexes[3] = Add(obb.center, Add(xAxis, Subtract(yAxis, zAxis)));
	vertexes[4] = Subtract(obb.center, Subtract(xAxis, Subtract(yAxis, zAxis)));
	vertexes[5] = Subtract(obb.center, Subtract(xAxis, Add(yAxis, zAxis)));
	vertexes[6] = Add(obb.center, Subtract(xAxis, Add(yAxis, zAxis)));
	vertexes[7] = Add(obb.center, Subtract(xAxis, Subtract(yAxis, zAxis)));
	Vector3 screenVertexes[8];
	for (int i = 0; i < 8; i++) {
		screenVertexes[i] = Transform(Transform(vertexes[i], viewProjectionMatrix), viewportMatrix);
	}
	for (int i = 0; i < 4; i++) {
		Novice::DrawLine(
			int(screenVertexes[i].x), int(screenVertexes[i].y),
			int(screenVertexes[(i + 1) % 4].x), int(screenVertexes[(i + 1) % 4].y),
			color);
		Novice::DrawLine(
			int(screenVertexes[i + 4].x), int(screenVertexes[i + 4].y),
			int(screenVertexes[(i + 1) % 4 + 4].x), int(screenVertexes[(i + 1) % 4 + 4].y),
			color);
		Novice::DrawLine(
			int(screenVertexes[i].x), int(screenVertexes[i].y),
			int(screenVertexes[i + 4].x), int(screenVertexes[i + 4].y),
			color);
	}

}