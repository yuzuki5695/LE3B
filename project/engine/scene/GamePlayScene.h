#pragma once
#include<Sprite.h>
#include<Object3d.h>
#include<Model.h>
#include<SoundPlayer.h>
#include<BaseScene.h>
#include <ParticleEmitter.h>

// ゲームプレイシーン
class GamePlayScene : public BaseScene
{
public: // メンバ関数
    // 初期化
    void Initialize() override;
    // 終了
    void Finalize() override;
    // 毎フレーム更新
    void Update() override;
    // 描画
    void Draw() override;
private: // メンバ変数
    
    std::shared_ptr <Sprite> sprite = nullptr;
    // Object3d
    std::shared_ptr<Object3d> object;
    // ファイルデータ
    // Model
    std::string ModelPath01;

    Transform transform_;
    // エミッター生成
    ParticleEmitter* emitter;
    Vector3 windPower;
};