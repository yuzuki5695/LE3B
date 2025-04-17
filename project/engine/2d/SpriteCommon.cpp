#include "SpriteCommon.h"
#include"GraphicsPipeline.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の定義
std::unique_ptr<SpriteCommon> SpriteCommon::instance = nullptr;

// シングルトンインスタンスの取得
SpriteCommon* SpriteCommon::GetInstance() {
    if (!instance) {
        instance = std::make_unique<SpriteCommon>();
    }
    return instance.get();
}


// 終了
void SpriteCommon::Finalize() {
    GraphicsPipeline::GetInstance()->Finalize();
    instance.reset();  // `delete` 不要
}

void SpriteCommon::Initialize(DirectXCommon* dxCommon) {
    // 引数を受け取ってメンバ変数に記録する
    dxCommon_ = dxCommon;
}

void SpriteCommon::Commondrawing() {
    // RootSignatureを設定。PSOに設定しているけど別途設定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetRootSignatureSprite().Get());
    dxCommon_->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetGraphicsPipelineStateSprite().Get());
    // 形状を設定。PSOに設定しているものとはまた別。同じものを設定する
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}