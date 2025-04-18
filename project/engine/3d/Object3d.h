#pragma once
#include"Model.h"
#include "Camera.h"
#include<memory>

class Object3dCommon;

//  3Dオブジェクト
class Object3d
{
public: // メンバ関数

	// 座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	// 平行光源データ
	struct DirectionalLight {
		Vector4 color; //!< ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity; //!< 輝度
	};
	// カメラデータ
	struct CameraForGPU
	{
		Vector3 worldPosition;
	};
public: // メンバ関数
	// 初期化
	void Initialize(Object3dCommon* object3dCommon);
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// 3Object作成関数
	static std::shared_ptr<Object3d> Create(std::string& filePath, Transform transform);
	// imgui
	void DebugUpdate();
private:
	// リソース
	// トランスフォームマトリックス
	void TransformationMatrixGenerate();
	// 平行光源リソース
	void DirectionalLightGenerate();
	// カメラリソース
	void CameraForGPUGenerate();
private:
	// ポインタ
	Object3dCommon* object3dCommon = nullptr;
	Model* model = nullptr;
	Camera* camera = nullptr;
	// バッファリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;
	DirectionalLight* directionalLightDate = nullptr;
	CameraForGPU* cameraForGPUData = nullptr;

	Transform transform_;
	Transform  cameratransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-800.0f} };
public:
	// getter
	const Transform& GetTransform() const { return transform_; }
	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }

	// setter
	void SetModel(const std::string& filePath);
	void SetScale(const Vector3& scale) { this->transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { this->transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { this->transform_.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
};