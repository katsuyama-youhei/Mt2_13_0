#include"matrix.h"
#include"World.h"
#include <Novice.h>

const char kWindowTitle[] = "LC1A_06_カツヤマヨウヘイ_確認課題";

struct Ball {
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	const int kWindowWidth = 480;
	const int kWindowHeight = 960;
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	const float kGravitiy = -9.8f;

	Ball ball[2]{
		Ball{
			{160.0f,960.0f},
			{0.0f,0.0f},
			{0.0f,kGravitiy},
			1.0f,
			10.0f,
			WHITE
		},
		Ball{
			{320.0f,960.0f},
			{0.0f,0.0f},
			{0.0f,kGravitiy},
			1.0f,
			10.0f,
			WHITE
		}
	};

	bool isShot = false;

	int image = Novice::LoadTexture("white1x1.png");

	Vector2 cameraPos = { 240.0f,480.0f };
	Vector2 orthoLeftTop = { -240.f,480.f };
	Vector2	orthoRightBottom = { 240.0f,-480.0f };
	Vector2	viewportLeftTop = { 0.0f,0.0f };
	Vector2	viewportSize = { kWindowWidth,kWindowHeight };

	const float k = 0.6f;

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

		if (keys[DIK_SPACE] != 0 && preKeys[DIK_SPACE]) {
			isShot = TRUE;
		}

		if (isShot == TRUE) {
			for (int i = 0; i < 2; ++i) {
				if (i == 0) {
					//空気抵抗は速度に比例して逆方向に発生する
					Vector2 airResistance = {
						k * -ball[i].velocity.x,
						k * -ball[i].velocity.y
					};
					//空気抵抗による加速度
					Vector2 airResistanceAcceleration = {
						airResistance.x / ball[i].mass,
						airResistance.y / ball[i].mass
					};
					ball[i].acceleration.y = kGravitiy + airResistanceAcceleration.y;
				}
				ball[i].velocity.x += (ball[i].acceleration.x / 60.0f);
				ball[i].velocity.y += (ball[i].acceleration.y / 60.0f);

				ball[i].position.x += (ball[i].velocity.x / 60.0f);
				ball[i].position.y += (ball[i].velocity.y / 60.0f);
			}
		}

		Matrix3x3 cameraMatrix = MakeAffineMatrix(Vector2{ 1.0f,1.0f }, 0.0f, cameraPos);
		Matrix3x3 viewMatrix = Inverse(cameraMatrix);
		Matrix3x3 orthoMatrix = MakeOrthographicMatrix(orthoLeftTop.x, orthoLeftTop.y, orthoRightBottom.x, orthoRightBottom.y);
		Matrix3x3 viewportMatrix = MakeViewPortMatrix(0, 0, viewportSize.x, viewportSize.y);
		Matrix3x3 vpMatrix = Multiply(viewMatrix, orthoMatrix);
		vpMatrix = Multiply(vpMatrix, viewportMatrix);

		//ボール変換
		Matrix3x3 worldMatrix[2];
		worldMatrix[0] = MakeAffineMatrix(Vector2{ 1.0f,1.0f }, 0.0f, ball[0].position);
		worldMatrix[1] = MakeAffineMatrix(Vector2{ 1.0f,1.0f }, 0.0f, ball[1].position);

		Matrix3x3 wvpMatrix[2];
		wvpMatrix[0] = Multiply(worldMatrix[0], vpMatrix);
		wvpMatrix[1] = Multiply(worldMatrix[1], vpMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawEllipse(
			wvpMatrix[0].m[2][0], wvpMatrix[0].m[2][1],
			ball[0].radius, ball[0].radius,
			0.0f, ball[0].color, kFillModeSolid
		);

		Novice::DrawEllipse(
			wvpMatrix[1].m[2][0], wvpMatrix[1].m[2][1],
			ball[1].radius, ball[1].radius,
			0.0f, ball[1].color, kFillModeSolid
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
