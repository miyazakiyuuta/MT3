#include "Matrix4x4.h"
#include <Novice.h>

Matrix4x4 MatrixMath::Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row][col] = m1.m[row][col] + m2.m[row][col];
		}
	}
	return result;
}

Matrix4x4 MatrixMath::Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row][col] = m1.m[row][col] - m2.m[row][col];
		}
	}
	return result;
}

Matrix4x4 MatrixMath::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row][col] =
				m1.m[row][0] * m2.m[0][col] +
				m1.m[row][1] * m2.m[1][col] +
				m1.m[row][2] * m2.m[2][col] +
				m1.m[row][3] * m2.m[3][col];
		}
	}
	return result;
}

Matrix4x4 MatrixMath::Inverse(const Matrix4x4& m) {
	float aug[4][8] = {};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			aug[row][col] = m.m[row][col];
		}
	}
	// 単位行列の追加
	aug[0][4] = 1.0f;
	aug[1][5] = 1.0f;
	aug[2][6] = 1.0f;
	aug[3][7] = 1.0f;

	for (int i = 0; i < 4; i++) {
		// ピボットが0の場合下の行と入れ替える
		if (aug[i][i] == 0.0f) {
			for (int j = i + 1; j < 4; j++) {
				if (aug[j][i] != 0.0f) {
					// 行を交換する
					for (int k = 0; k < 8; k++) { // 列
						float copyNum = aug[i][k]; //もともとの上の行を代入
						aug[i][k] = aug[j][k]; //上の行
						aug[j][k] = copyNum; //下の行
					}
					break;
				}
			}
		}

		// ピボットを1にする
		float pivot = aug[i][i];
		for (int k = 0; k < 8; k++) {
			aug[i][k] /= pivot;
		}

		for (int j = 0; j < 4; j++) {
			if (j != i) {
				float factor = aug[j][i];
				for (int k = 0; k < 8; k++) {
					aug[j][k] -= factor * aug[i][k];
				}
			}
		}

	}

	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row][col] = aug[row][col + 4];
		}
	}

	return result;
}

Matrix4x4 MatrixMath::Transpose(const Matrix4x4& m) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row][col] = m.m[col][row];
		}
	}
	return result;
}

Matrix4x4 MatrixMath::MakeIdentity4x4() {
	Matrix4x4 result = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	return result;
}

void MatrixMath::MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char type[]) {
	Novice::ScreenPrintf(x, y, "%s", type);
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}





