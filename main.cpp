#include <Novice.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include <cmath>
#include <imgui.h>

const char kWindowTitle[] = "LE2B_24_ミヤザキ_ユウタ_タイトル";
const int kWindowWidth = 1280; // 画面の横幅
const int kWindowHeight = 720; // 画面の縦幅

const float pi = 3.14159265f;

using namespace MatrixMath;
using namespace Vector3Math;

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
	unsigned int color;
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate = { 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };

	Sphere sphere[2] = {
		{{0.0f,0.0f,0.0f},0.5f,0xFFFFFFFF},
		{{1.0f,1.0f,1.0f},0.5f,0xFFFFFFFF}
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
		
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate,cameraTranslate );
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(3.14f / 4.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		if (IsCollision(sphere[0], sphere[1])) {
			sphere[0].color = RED;
		} else {
			sphere[0].color = WHITE;
		}

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("SphereCenter[0]", &sphere[0].center.x, 0.01f);
		ImGui::DragFloat("SphereRadius[0]", &sphere[0].radius, 0.01f);
		ImGui::DragFloat3("SphereCenter[1]", &sphere[1].center.x, 0.01f);
		ImGui::DragFloat("SphereRadius[1]", &sphere[1].radius, 0.01f);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		 
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		for(int i=0;i<2;i++){
			DrawSphere(sphere[i], viewProjectionMatrix, viewportMatrix, sphere[i].color);
		}
		

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