#include <Novice.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include <cstdint>

const char kWindowTitle[] = "LE2B_24_ミヤザキ_ユウタ_タイトル";
const int kWindowWidth = 1280; // 画面の横幅
const int kWindowHeight = 720; // 画面の縦幅

using namespace Vector3Math;
using namespace MatrixMath;

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char type[]);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 rotate = {};
	Vector3 translate = {};

	//float radian = 0.0f;

	Vector3 cameraPosition = {};

	const Vector3 kLocalVertices[3] = {
		{0.0f,0.1f,0.5f},
		{0.1f,-0.1f,0.5f},
		{-0.1f,-0.1f,0.5f}
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

		Vector3 v1{ 1.2f,-3.9f,2.5f };
		Vector3 v2{ 2.8f,0.4f,-1.3f };
		Vector3 cross = Cross(v1, v2);

		if (keys[DIK_W]) {
			translate.z += 0.01f;
		}
		if (keys[DIK_S]) {

			translate.z -= 0.01f;
		}
		if (keys[DIK_A]) {
			translate.x -= 0.01f;
		}
		if (keys[DIK_D]) {
			translate.x += 0.01f;
		}
		Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

		// Y軸回転
		rotate.y += 0.1f;

		// 軸にしたい頂点を原点にする行列
		Matrix4x4 moveToOriginMatrix = MakeTranslateMatrix({ -kLocalVertices[0].x,-kLocalVertices[0].y,-kLocalVertices[0].z });
		// 回転行列
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(rotate.y);
		// 原点にした処理を戻す行列
		Matrix4x4 moveBackMatrix = MakeTranslateMatrix(kLocalVertices[0]);

		//Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 worldMatrix = Multiply(Multiply(Multiply(moveToOriginMatrix, rotateMatrix), moveBackMatrix), translateMatrix);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f, }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, cross, "Cross");

		Novice::DrawTriangle(
			int(screenVertices[0].x), int(screenVertices[0].y),
			int(screenVertices[1].x), int(screenVertices[1].y),
			int(screenVertices[2].x), int(screenVertices[2].y),
			RED, kFillModeSolid
		);

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