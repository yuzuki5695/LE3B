#include "ParticleManager.h"
#include <MatrixVector.h>
#include <cassert>
#include <ModelManager.h>
#include <TextureManager.h>
#include <numbers>
#include<ImGuiManager.h>
#include <externals/DirectXTex/d3dx12.h>
#include <Object3dCommon.h>

using namespace MatrixVector;
using namespace Microsoft::WRL;

// 静的メンバ変数の定義
std::unique_ptr<ParticleManager> ParticleManager::instance = nullptr;

// シングルトンインスタンスの取得
ParticleManager* ParticleManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<ParticleManager>();
    }
    return instance.get();
}

// 終了
void ParticleManager::Finalize() {
    instance.reset();
}

void ParticleManager::Initialize(DirectXCommon* birectxcommon, SrvManager* srvmanager) {
    // NULL検出
    assert(birectxcommon);
    // メンバ変数に記録
    this->dxCommon_ = birectxcommon;
    this->srvmanager_ = srvmanager;
    camera_ = Object3dCommon::GetInstance()->GetDefaultCamera();
    // 乱数エンジンを初期化
    std::random_device rd;// 乱数生成器
    randomEngine = std::mt19937(rd());
    // マテリアルの生成と初期化
    MaterialGenerate();
    //ビルボード行列作成
    backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
}

void ParticleManager::Update() {
    Matrix4x4 billboardMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    // ビルボード行列: パーティクルがカメラに向くように変換
    if (camera_) {
        // カメラのワールド行列を取得し、ビルボード行列を計算
        billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorludMatrix());  // 修正: GetWorldMatrix
        // カメラからビュー行列とプロジェクション行列を取得
        viewMatrix = camera_->GetViewMatrix();
        projectionMatrix = camera_->GetProjectionMatrix();
    } else {
        // カメラがない場合の処理（必要であれば）
    }

    // パーティクルの位置をカメラの方向に合わせるために設定
    billboardMatrix.m[3][0] = 0.0f;
    billboardMatrix.m[3][1] = 0.0f;
    billboardMatrix.m[3][2] = 0.0f;

    // 各パーティクルグループの処理
    for (auto& [name, group] : particleGroups) {
        uint32_t counter = 0;
        for (auto particleIterator = group.particles.begin(); particleIterator != group.particles.end();) {
            // パーティクルの更新処理（時間、移動など）
            particleIterator->currentTime += 1.0f / 60.0f;  // 60fpsで時間をカウントアップ

            // 残り時間に応じた明度（1.0〜0.0）
            float progress = particleIterator->currentTime / particleIterator->lifetime;
            float brightness = std::clamp(1.0f - progress, 0.0f, 1.0f);

            if (particleIterator->currentTime >= particleIterator->lifetime) {
                particleIterator = group.particles.erase(particleIterator);
                continue;
            }

            // ランダムに設定された方向に移動
            particleIterator->transform.translate.x += particleIterator->Velocity.x;
            particleIterator->transform.translate.y += particleIterator->Velocity.y;

            // world行列の計算
            Matrix4x4 localWorld = MakeAftineMatrix(
                particleIterator->transform.scale,
                particleIterator->transform.rotate,
                particleIterator->transform.translate
            );
            
            Matrix4x4 worldMatrix = Multiply(localWorld, billboardMatrix);  // ビルボードと合成

            // worldViewProjection行列の計算
            Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

            // インスタンスデータに更新した行列を設定
            if (counter < group.kNumInstance) {
                group.instanceData[counter].WVP = worldViewProjectionMatrix;
                group.instanceData[counter].World = worldMatrix;
                group.instanceData[counter].color = (*particleIterator).color;
                particleIterator->color.w = brightness;  // アルファ値だけ落とす
                ++counter;
            }

            // 次のパーティクルに進む
            ++particleIterator;
        }

        //// パーティクル発生の処理（一定時間ごとに新しいパーティクルを生成）
        //if (group.spawnTime >= group.spawnFrequency) {
        //    // 新しいパーティクルを発生させる
        //    Emit("Particles", Vector3{ 0.0f, 0.0f, 0.0f }, 3);  // 実際の発生処理を呼び出す
        //    group.spawnTime = 0.0f;  // 発生時間をリセット
        //} else {
        //    // 発生時間を増加
        //    group.spawnTime += 1.0f / 60.0f;
        //}
    }
}

void ParticleManager::Commondrawing() {
    // RootSignature と PipelineState を設定
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetRootSignatureParticle().Get());
    dxCommon_->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetGraphicsPipelineStateParticle().Get());
    // 形状を設定。PSOに設定しているものとはまた別。同じものを設定する
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleManager::Draw() {

    // パーティクルグループごとに描画処理を行う
    for (const auto& [name, particleGroup] : particleGroups) {
        // インスタンス数が0の場合は描画しない
        if (particleGroup.kNumInstance == 0) {
            continue;
        }
        // VertexBufferView を設定
        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        // マテリアル用の定数バッファを設定
        dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetSrvHandleGPU(particleGroup.materialData.textureFilePath));
        // インスタンシングデータの SRV を設定（テクスチャファイルのパスを指定）
        srvmanager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvindex);
        // SRVで画像を表示
        srvmanager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureindex);
        // 描画（インスタンシング）を実行
        dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(modelDate.vertices.size()), static_cast<UINT>(particleGroup.kNumInstance), 0, 0);
    }
}

void ParticleManager::VertexDatacreation() {
    // 関数化したResouceで作成
    vertexResoruce = dxCommon_->CreateBufferResource(sizeof(VertexData) * modelDate.vertices.size());
    //頂点バッファビューを作成する
    // リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResoruce->GetGPUVirtualAddress();
    // 使用するリソースのサイズはの頂点のサイズ
    vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelDate.vertices.size());
    // 1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);
    // 頂点リソースにデータを書き込むためのアドレスを取得
    vertexResoruce->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    // 頂点データをリソースにコピー
    std::memcpy(vertexData, modelDate.vertices.data(), sizeof(VertexData) * modelDate.vertices.size());
}

void ParticleManager::MaterialGenerate() {
    // マテリアル用のリソース
    materialResource = dxCommon_->CreateBufferResource(sizeof(Model::Material));
    // マテリアル用にデータを書き込むためのアドレスを取得
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    // マテリアルデータの初期値を書き込む
    materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    materialData->endbleLighting = true;
    materialData->uvTransform = MakeIdentity4x4();
}

void ParticleManager::CreateParticles(const std::string& directorypath, const std::string& filename) {
    // モデルデータを取得
    modelDate = LoadObjFile(directorypath, filename);
    // 頂点データを作成
    VertexDatacreation();
    // .objの参照しているテクスチャ読み込み
    TextureManager::GetInstance()->LoadTexture(modelDate.material.textureFilePath);
    // 読み込んだテクスチャの番号を取得
    modelDate.material.textureindex = TextureManager::GetInstance()->GetSrvIndex(modelDate.material.textureFilePath);
}


void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilepath) {
    // すでにテクスチャがロードされているか確認
    if (!TextureManager::GetInstance()->IsTextureLoaded(textureFilepath)) {
        // マテリアルのテクスチャファイルをロード
        TextureManager::GetInstance()->LoadTexture(textureFilepath);
    }

    // パーティクルグループ名が既に存在するかチェック
    auto it = particleGroups.find(name);
    if (it != particleGroups.end()) {
        // 名前が一致するグループが存在する場合、そのグループのテクスチャが一致するか確認
        if (it->second.materialData.textureFilePath == textureFilepath) {
            // テクスチャが一致する場合、既存のグループを再利用
            return;
        } else {
            // テクスチャが異なる場合、既存のグループを更新
            it->second.materialData.textureFilePath = textureFilepath;
            it->second.materialData.textureindex = TextureManager::GetInstance()->GetSrvIndex(textureFilepath);
            // 必要に応じてリソースの再割り当てなどを行うことができます
        }
    } else {
        // 新しいパーティクルグループを作成
        ParticleGroup& newGroup = particleGroups[name];

        // 新しいパーティクルグループにテクスチャパスとインデックスを設定
        newGroup.materialData.textureFilePath = textureFilepath;
        newGroup.materialData.textureindex = TextureManager::GetInstance()->GetSrvIndex(textureFilepath);

        // インスタンス用のリソースバッファを作成
        newGroup.Resource = dxCommon_->CreateBufferResource(sizeof(InstanceData) * MaxInstanceCount);
        newGroup.instanceData = nullptr;
        newGroup.Resource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.instanceData));

        // インスタンスデータを初期化
        for (uint32_t index = 0; index < MaxInstanceCount; ++index) {
            newGroup.instanceData[index].WVP = MakeIdentity4x4();
            newGroup.instanceData[index].World = MakeIdentity4x4();
            newGroup.instanceData[index].color = { 1.0f, 1.0f, 1.0f, 0.0f }; // 透明
        }
        // 書き込み後にリソースをアンマップ
        newGroup.Resource->Unmap(0, nullptr);
        // インスタンスバッファ用のSRVを割り当て、インデックスを記録
        newGroup.srvindex = srvmanager_->Allocate();
        // 構造体バッファ用のSRVを作成
        srvmanager_->CreateSRVforStructuredBuffer(newGroup.srvindex, newGroup.Resource.Get(), MaxInstanceCount, sizeof(InstanceData));
    }
}

ParticleManager::MaterialDate ParticleManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
    // 1. 中で必要となる変数の宣言
    MaterialDate materialDate; // 構築するMaterialDate
    std::string line; // ファイルから読んだ1行を格納するもの
    std::ifstream file(directoryPath + "/" + filename); // 2.ファイルを開く
    assert(file.is_open()); // とりあえず開けなかったら止める
    // 3. 実際にファイルを読み、MaterialDateを構築していく
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // identifierの応じた処理
        if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            // 連結してファイルパスにする
            materialDate.textureFilePath = directoryPath + "/" + textureFilename;
        }
    }
    return materialDate;
}

ParticleManager::ModelDate ParticleManager::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    // 1. 中で必要となる変数の宣言
    ModelDate modelDate; // 構築するModelDate
    std::vector<Vector4> positions; // 位置
    std::vector<Vector3> normals; // 法線
    std::vector<Vector2> texcoords; // テクスチャ座標
    std::string line; // ファイルから読んだ1桁を格納するもの
    // 2.  ファイルを開く
    std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
    assert(file.is_open()); // とりあえず開けなかったら止める

    // 3. 実際にファイルを読み、ModelDateを構築していく
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;// 先頭の識別子を読む

        // identifierの応じた処理
        if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.x *= -1.0f;// 位置のx成分を反転
            position.w = 1.0f;
            positions.push_back(position);
        } else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        } else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normal.x *= -1.0f;// 法線のx成分を反転
            normals.push_back(normal);
        } else if (identifier == "f") {
            VertexData triangle[3];
            // 面は三角形限定。その他は未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                // 頂点の要素へのIndexは、[位置/UV/法線]で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];
                for (uint32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');// /区切りでインデックスを読んでいく
                    elementIndices[element] = std::stoi(index);
                }
                // 要素のIndexから、実際の要素の値を取得して、頂点を構築する
                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texcoord = texcoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];
                //VertexData vertex = { position,texcoord,normal };
                //modelDate.vertices.push_back(vertex);
                triangle[faceVertex] = { position,texcoord,normal };
            }
            // 頂点を逆順で登録することで、回り順を逆にする
            modelDate.vertices.push_back(triangle[2]);
            modelDate.vertices.push_back(triangle[1]);
            modelDate.vertices.push_back(triangle[0]);
        } else if (identifier == "mtllib") {
            // materialTemplateLibrarvファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            // 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
            modelDate.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }
    // 4. ModelDateを返す
    return modelDate;
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count, const Vector3& Velocity) {
    auto it = particleGroups.find(name);
    if (it == particleGroups.end()) {
        assert(false);
        return;
    }

    ParticleGroup& group = it->second;

    size_t currentParticleCount = group.particles.size();
    if (currentParticleCount + count > MaxInstanceCount) {
        count = static_cast<uint32_t>(MaxInstanceCount - currentParticleCount);
    }

    std::uniform_real_distribution<float> dist(0.0f, 0.0f);
    std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
    std::uniform_real_distribution<float> distVelocity(-0.001f, 0.001f); 
    std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> distScale(0.4f,1.5f);

    for (uint32_t i = 0; i < count; ++i) {
        Vector3 offset(dist(randomEngine), dist(randomEngine), dist(randomEngine));
        Vector4 color = Vector4(distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f);
        Vector3 rotate = Vector3(0.0f, 0.0f, distRotate(randomEngine));
        Vector3 scale = Vector3(0.05f, distScale(randomEngine), 1.0f);

        Particle newParticle;
        newParticle.transform.translate = { position.x + offset.x, position.y + offset.y, position.z + offset.z };
        newParticle.transform.rotate = rotate;
        newParticle.transform.scale = scale;
        newParticle.color = color;
        newParticle.lifetime = 3.0f;
        newParticle.currentTime = 0.0f;
        newParticle.Velocity = Vector3(0.0f,0.0f,0.0f);

        //// 風の影響（X方向）を追加
        //float randomX = distVelocity(randomEngine);
        //newParticle.Velocity = {
        //    Velocity.x + randomX,  // ← 風 + 微調整
        //    distVelocity(randomEngine),
        //    0.0f
        //};

        group.particles.push_back(newParticle);
    }

    // インスタンス数をパーティクル数に揃える
    group.kNumInstance = static_cast<uint32_t>(group.particles.size());
}

void ParticleManager::DebugUpdata() {
#ifdef USE_IMGUI

#endif // USE_IMGUI
}