#pragma once
#include"Transform.h"
#include"Matrix4x4.h"

// カメラ
class Camera
{
public: // メンバ関数
	// コントラクタ
	Camera();
	// 更新処理
	void Update();
	// ImGuiの更新
	void DebugUpdata();
private: // メンバ変数
	Transform  transform;
	Matrix4x4 worludMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	float fovY; // 水平方向視野野角
	float aspectRatio; // アスペクト比
	float nearClip; // ニアクリップ距離
	float farclip; // ファークリップ距離
	Matrix4x4 ViewProjectionMatrix;
public:
	// getter
	const Matrix4x4& GetWorludMatrix() const { return worludMatrix; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
	const Matrix4x4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }

	// setter
	void SetRotate(const Vector3& rotate) { this->transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { this->transform.translate = translate; }
	void SetFovY(const float& fovY) { this->fovY = fovY; }
	void SetAspectRatio(const float& aspectRatio) { this->aspectRatio = aspectRatio; }
	void SetNearClip(const float& nearClip) { this->nearClip = nearClip; }
	void Setfarclip(const float& farclip) { this->farclip = farclip; }
};