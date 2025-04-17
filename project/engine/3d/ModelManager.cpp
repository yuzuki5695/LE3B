#include "ModelManager.h"

// 静的メンバ変数の定義
std::unique_ptr<ModelManager> ModelManager::instance = nullptr;

// シングルトンインスタンスの取得
ModelManager* ModelManager::GetInstance() {
	if (!instance) {
		instance = std::make_unique<ModelManager>();
	}
	return instance.get();
}

// 終了
void ModelManager::Finalize() {
	instance.reset();
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	// NULL検出
	assert(dxCommon);
	// 3Dモデル共通部の初期化
	// メンバ変数に記録
	modelCommon = new ModelCommon;
	modelCommon->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath) {
	//  読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}
	// モデルの生成とファイル読み込み,初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelCommon, "Resources", filePath);
	// モデルをmapコンテナに格納する
	models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath) {
	//  読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込みモデルを戻り値としてreturn
		return  models.at(filePath).get();
	}
	// ファイル名一致なし
	return nullptr;
};