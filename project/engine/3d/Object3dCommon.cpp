#include "Object3dCommon.h"
#include <cassert>
#include "GraphicsPipeline.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の定義
std::unique_ptr<Object3dCommon> Object3dCommon::instance = nullptr;

// シングルトンインスタンスの取得
Object3dCommon* Object3dCommon::GetInstance() {
    if (!instance) {
        instance = std::make_unique<Object3dCommon>();
    }
    return instance.get();
}


// 終了
void Object3dCommon::Finalize() {
    GraphicsPipeline::GetInstance()->Finalize();
    instance.reset();  // `delete` 不要
}


void Object3dCommon::Initialize(DirectXCommon* dxCommon) {
    // NULL検出
    assert(dxCommon);
    // 引数を受け取ってメンバ変数に記録する
    dxCommon_ = dxCommon;
}

void Object3dCommon::Commondrawing() {
    // RootSignatureを設定。PSOに設定しているけど別途設定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetRootSignatureObject3d().Get());
    dxCommon_->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetGraphicsPipelineStateObject3d().Get());
    // 形状を設定。PSOに設定しているものとはまた別。同じものを設定する
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}