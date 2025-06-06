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
	uint32_t color;
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

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char type[]);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, unsigned int color);

Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

bool IsCollision(const Sphere& s1, const Sphere& s2);
bool IsCollision(const Sphere& sphere, const Plane& plane);

Vector3 Perpendicular(const Vector3& vector);

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

bool IsCollision(const Segment& segment, const Plane& plane);

bool IsCollision(const Triangle& triangle, const Segment& segment);

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

bool IsCollision(const AABB& aabb, const Sphere& sphere);

bool IsCollision(const AABB& aabb, const Segment& segment);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int mouseX;
	int mouseY;
	int prevMouseX = 0;
	int prevMouseY = 0;

	Vector3 cameraTranslate = { 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };

	AABB aabb{
		.min{-0.5f,-0.5f,-0.5f},
		.max{0.5f,0.5f,0.5f},
		.color{WHITE},
	};
	Segment segment{
		.origin{-0.7f,0.3f,0.0f},
		.diff{2.0f,-0.5f,0.0f},
		.color{WHITE},
	};

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

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(3.14f / 4.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		if (IsCollision(aabb, segment)) {
			aabb.color = RED;
		} else {
			aabb.color = WHITE;
		}

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("aabb.min", &aabb.min.x, 0.01f);
		ImGui::DragFloat3("aabb.max", &aabb.max.x, 0.01f);
		ImGui::DragFloat3("segment.origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);
		ImGui::End();

		// カメラ
		Novice::GetMousePosition(&mouseX, &mouseY);
		if (Novice::IsPressMouse(0)) {

			// マウスの移動量(
			int deltaX = mouseX - prevMouseX;
			int deltaY = mouseY - prevMouseY;
			// 感度
			float sensitivity = 0.001f;
			// カメラの回転に反映
			cameraRotate.y += float(deltaX) * sensitivity;
			cameraRotate.x += float(deltaY) * sensitivity;
		}
		// 現在のマウス位置を保存
		prevMouseX = mouseX;
		prevMouseY = mouseY;

		float speed = float(Novice::GetWheel()) * 0.01f;
		Vector3 direction = { cameraRotate.y,cameraRotate.z,cameraRotate.x };

		cameraTranslate = Add(cameraTranslate, Multiply(speed, direction));

		aabb.min.x = min(aabb.min.x, aabb.max.x);
		aabb.max.x = max(aabb.min.x, aabb.max.x);
		aabb.min.y = min(aabb.min.y, aabb.max.y);
		aabb.max.y = max(aabb.min.y, aabb.max.y);
		aabb.min.z = min(aabb.min.z, aabb.max.z);
		aabb.max.z = max(aabb.min.z, aabb.max.z);

		Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);

		DrawAABB(aabb, viewProjectionMatrix, viewportMatrix, aabb.color);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), segment.color);

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

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
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

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && // x軸
		aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y && // y軸
		aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) { // z軸
		return true;
	}
	return false;
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertex[8];
	vertex[0] = aabb.min;
	vertex[1] = { aabb.max.x,aabb.min.y,aabb.min.z };
	vertex[2] = { aabb.max.x,aabb.min.y,aabb.max.z };
	vertex[3] = { aabb.min.x,aabb.min.y,aabb.max.z };
	vertex[4] = { aabb.min.x,aabb.max.y,aabb.min.z };
	vertex[5] = { aabb.max.x,aabb.max.y,aabb.min.z };
	vertex[6] = aabb.max;
	vertex[7] = { aabb.min.x,aabb.max.y,aabb.max.z };
	Vector3 screenVertex[8];
	for (int i = 0; i < 8; i++) {
		screenVertex[i] = Transform(Transform(vertex[i], viewProjectionMatrix), viewportMatrix);
	}
	for (int i = 0; i < 4; i++) {
		Novice::DrawLine(
			int(screenVertex[i].x), int(screenVertex[i].y),
			int(screenVertex[(i + 1) % 4].x), int(screenVertex[(i + 1) % 4].y),
			color);
		Novice::DrawLine(
			int(screenVertex[i + 4].x), int(screenVertex[i + 4].y),
			int(screenVertex[(i + 1) % 4 + 4].x), int(screenVertex[(i + 1) % 4 + 4].y),
			color);
		Novice::DrawLine(
			int(screenVertex[i].x), int(screenVertex[i].y),
			int(screenVertex[i + 4].x), int(screenVertex[i + 4].y),
			color);
	}
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