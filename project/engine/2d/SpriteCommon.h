#pragma once
#include"DirectXCommon.h"
#include<GraphicsPipeline.h>

// スプライト共通部
class SpriteCommon
{
private:  
	static std::unique_ptr<SpriteCommon> instance;
	
	SpriteCommon(SpriteCommon&) = delete;
	SpriteCommon& operator=(SpriteCommon&) = delete;
public: // メンバ関数
	SpriteCommon() = default;
	~SpriteCommon() = default;

	// シングルトンインスタンスの取得
	static SpriteCommon* GetInstance();
	// 終了
	void Finalize();
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// 共通描画設定
	void Commondrawing();

	DirectXCommon* GetDxCommon() const { return  dxCommon_; }
private:
	// ポインタ
	DirectXCommon* dxCommon_;
};