#pragma once
#include"DirectXCommon.h"
#include"Camera.h"
#include<GraphicsPipeline.h>

// 3Dオブジェクト共通部
class Object3dCommon
{
private:
	static std::unique_ptr<Object3dCommon> instance;

	Object3dCommon(Object3dCommon&) = delete;
	Object3dCommon& operator=(Object3dCommon&) = delete;
public: // メンバ関数
	Object3dCommon() = default;
	~Object3dCommon() = default;

	// シングルトンインスタンスの取得
	static Object3dCommon* GetInstance();
	// 終了
	void Finalize();
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// 共通描画設定
	void Commondrawing();
private:
	// ポインタ
	DirectXCommon* dxCommon_;
	Camera* defaultCamera = nullptr;
public:
	// setter
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	// getter
	DirectXCommon* GetDxCommon() const { return  dxCommon_; }
	Camera* GetDefaultCamera() const { return defaultCamera; }
};