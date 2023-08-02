#include <Novice.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include <algorithm>
#include <imgui.h>
#define _USE_MATH_DEFINES

#include "Mt4.h"

const char kWindowTitle[] = "LE2C_13_サイトウ_ユウキ_MT3_02_00";

//Grid
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

struct Sphere {
	Vector3 center; //!< 中心点
	float radius;   //!< 半径
};

//Sphere
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

using namespace std;

struct Line {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Ray {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Segment segment{ {-2.0f,-1.0f,0.0f},{3.0f,2.0f,2.0f} };
	Vector3 point{ -1.5f,0.6f,0.6f };

	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);

	Sphere pointSphere{ point,0.01f }; // 1cmの球を描画
	Sphere closestPointSphere{ closestPoint,0.01f };

	float kWindowWidth = 1280.0f;
	float kWindowHeight = 720.0f;

	Vector3 rotate{};
	Vector3 translate{};

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	Sphere sphere = { 0.0f,0.0f,0.0f,0.5f };

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
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewPortMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewPortMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewPortMatrix);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);

		DrawSphere(pointSphere, viewProjectionMatrix, viewPortMatrix, RED);
		DrawSphere(closestPointSphere, viewProjectionMatrix, viewPortMatrix, BLACK);

		DrawGrid(viewProjectionMatrix, viewPortMatrix);

		ImGui::Begin("Window");
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
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

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float v2Length = Dot(v2, v2);
	float dot = Dot(v1, v2);

	return Multiply(dot / v2Length, v2);
}

//segment.diff b

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 a = Subtract(point,segment.origin);
	
	float length = Dot(segment.diff, segment.diff);
	float dot = Dot(a, segment.diff);

	float t = dot / length;

	t = clamp(t,0.0f,1.0f);

	Vector3 pro = Multiply(t, segment.diff);

	return Add(segment.origin, pro);
}

//Grid
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f; //Gridの半分の幅
	const uint32_t kSubdivision = 10; //分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); //1つ分の長さ
	//奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);

		//上の情報を使ってワールド座標系上の始点と終点を求める
		Vector3 start = { x,0.0f,-kGridHalfWidth };
		Vector3 end = { x,0.0f,kGridHalfWidth };

		//スクリーン座標系まで変換をかける
		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		//変換した座標を使って表示。色は薄い灰色(0xAAAAAAFF),原点は黒ぐらいが良いが、何でも良い
		if (x == 0) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0x000000FF);
		}
		else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0xAAAAAAFF);
		}
	}
	//左から右も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);

		//上の情報を使ってワールド座標系上の始点と終点を求める
		Vector3 start = { -kGridHalfWidth,0.0f,z };
		Vector3 end = { kGridHalfWidth,0.0f,z };

		//スクリーン座標系まで変換をかける
		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		//変換した座標を使って表示。色は薄い灰色(0xAAAAAAFF),原点は黒ぐらいが良いが、何でも良い
		if (z == 0) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0x000000FF);
		}
		else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0xAAAAAAFF);
		}
	}
}

//Sphere
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 12; //分割数
	const float kLonEvery = 2.0f * float(M_PI) / kSubdivision; //経度分割1つ分の角度 φ
	const float kLatEvery = float(M_PI) / kSubdivision;  //緯度分割1つ分の角度 θ
	//緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; //現在の緯度 θ
		//経度の方向に分割　0~2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; //現在の経度  φ
			//world座標系でのa,b,cを求める
			Vector3 a
			{
				sphere.radius * cosf(lat) * cosf(lon) + sphere.center.x,
				sphere.radius * sinf(lat) + sphere.center.y,
				sphere.radius * cosf(lat) * sinf(lon) + sphere.center.z
			};
			Vector3 b
			{
				sphere.radius * cosf(lat + kLatEvery) * cosf(lon) + sphere.center.x,
				sphere.radius * sinf(lat + kLatEvery) + sphere.center.y,
				sphere.radius * cosf(lat + kLatEvery) * sinf(lon) + sphere.center.z
			};
			Vector3 c
			{
				sphere.radius * cosf(lat) * cosf(lon + kLonEvery) + sphere.center.x,
				sphere.radius * sinf(lat) + sphere.center.y,
				sphere.radius * cosf(lat) * sinf(lon + kLonEvery) + sphere.center.z
			};
			// a,b,cをScreen座標系まで変換
			Vector3 screenA = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 screenB = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 screenC = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

			//ab,bcで線を引く
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}