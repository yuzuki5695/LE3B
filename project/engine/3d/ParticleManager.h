#pragma once
#include<DirectXCommon.h>
#include<SrvManager.h>
#include<random>
#include<Vector2.h>
#include<Vector3.h>
#include<Vector4.h>
#include<Matrix4x4.h>
#include <Transform.h>
#include<Camera.h>
#include<Model.h>
#include<GraphicsPipeline.h>
#include <ParticleEmitter.h>

// パーティクルマネージャ
class ParticleManager
{
private:
	static std::unique_ptr<ParticleManager> instance;

	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;
public:

	// 頂点データ
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	// マテリアル
	struct Material {
		Vector4 color;
		int32_t endbleLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	// マテリアルデータ
	struct MaterialDate {
		std::string textureFilePath;
		uint32_t textureindex = 0;
	};

	// モデルデータ
	struct ModelDate {
		std::vector<VertexData> vertices;
		MaterialDate material;
	};

	// パーティクル
	struct Particle {
		Transform transform;
		Vector3 Velocity;
		float lifetime;
		float currentTime;
		Vector4 color;
	};

	// インスタンスデータ
	struct InstanceData
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	// パーティクルグループ
	struct ParticleGroup {
		MaterialDate materialData;                             // マテリアルデータ(テクスチャファイルパスとテクスチャ用SRVインデックス)
		std::list<Particle> particles;                         // パーティクルのリスト
		uint32_t srvindex;                                     // インスタンシング用SRVインデックス
		Microsoft::WRL::ComPtr <ID3D12Resource> Resource;      // インスタンシングリソース
		uint32_t kNumInstance;                                 // インスタンス数
		InstanceData* instanceData = nullptr;                  // インスタンシングデータを書き込むためのポインタ
		// 発生間隔管理用のメンバー
		float spawnTime = 0.0f;  // 現在の発生までの経過時間
		float spawnFrequency = 1.0f;  // 1.0秒ごとに発生
	};
private:
	// 頂点データ作成
	void VertexDatacreation();
	// マテリアルデータ作成
	void MaterialGenerate();
public: // メンバ関数
	ParticleManager() = default;
	~ParticleManager() = default;

	// シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
	// 終了
	void Finalize();
	// 初期化
	void Initialize(DirectXCommon* birectxcommon, SrvManager* srvmanager);
	// 更新処理
	void Update();
	// 描画処理
	void Draw();
	// 共通描画設定
	void Commondrawing();

	// パーティクルグループの作成
	void CreateParticleGroup(const std::string& name, const std::string& textureFilepath);

	// .mtlファイルの読み取り
	static ParticleManager::MaterialDate LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	// .objファイルの読み取り
	static ParticleManager::ModelDate LoadObjFile(const std::string& directoryPath, const std::string& filename);

	void Emit(const std::string name, const Vector3& position, uint32_t count, const Vector3& Velocity);

	void CreateParticles(const std::string& directorypath, const std::string& filename);

	void DebugUpdata();

private: // メンバ変数
	// ポインタ
	DirectXCommon* dxCommon_;
	SrvManager* srvmanager_;
	Camera* camera_;
	// ランダムエンジン
	std::mt19937 randomEngine;
	// Objファイルのデータ
	ModelDate modelDate;
	// バッファリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResoruce;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	// パーティクルグループコンテナ
	std::unordered_map<std::string, ParticleGroup> particleGroups;
	//最大インスタンス
	uint32_t MaxInstanceCount = 100;
	//ビルボード行列
	Matrix4x4 backToFrontMatrix;
	//modelマテリアる用のリソースを作る。今回color1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む	
	Material* materialData = nullptr;

	// RootSignature
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
};