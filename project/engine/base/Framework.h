#pragma once
#include<Input.h>
#include<DirectXCommon.h>
#include<ResourceObject.h>
#include<D3DResourceLeakChecker.h>
#include<TextureManager.h>
#include<SpriteCommon.h>
#include<Object3dCommon.h>
#include<ModelCommon.h>
#include<ModelManager.h>
#include<Camera.h>
#include<SrvManager.h>
#include<ImGuiManager.h>
#include<SoundPlayer.h>
#include<SceneManager.h>
#include "SceneFactory.h"
#include"GraphicsPipeline.h"
#include"ParticleManager.h"
#include<memory>

// ゲーム全体
class Framework
{
public:// メンバ関数
	// デストラクタ
	virtual ~Framework() = default;

	// 初期化
	virtual void Initialize();
	// 終了
	virtual void Finalize();
	// 毎フレーム更新
	virtual void Update();
	// 描画
	virtual void Draw() = 0;

	// 実行
	void Run();

protected:
	// シーンファクトリー
	std::unique_ptr <AbstractSceneFactory> sceneFactory_ = nullptr;
private: // メンバ変数
	// ポインタ
	std::unique_ptr <WinApp> winApp = nullptr;
	std::unique_ptr<DirectXCommon> dxCommon = nullptr;
	std::unique_ptr <ModelCommon> modelCommon = nullptr;
	std::unique_ptr <SrvManager> srvManager = nullptr;
	std::unique_ptr <Camera> camera = nullptr;

	// カメラの現在の位置と回転を取得
	Vector3 Cameraposition;
	Vector3 Camerarotation;

	// ゲーム終了フラグ
	bool endRequst_ = false;
public:
	// getter 
	DirectXCommon* GetDirectXCommon() const { return dxCommon.get(); }
	// 終了フラグのチェック
	virtual bool IsEndRequst() { return  endRequst_; }
};