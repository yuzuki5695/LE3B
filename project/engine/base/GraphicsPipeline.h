#pragma once
#include "DirectXCommon.h"

class GraphicsPipeline
{
private:
	static std::unique_ptr<GraphicsPipeline> instance;

	GraphicsPipeline(GraphicsPipeline&) = delete;
	GraphicsPipeline& operator=(GraphicsPipeline&) = delete;
public: // メンバ関数
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

	// シングルトンインスタンスの取得
	static GraphicsPipeline* GetInstance();
	// 終了
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);
	void GenerateSprite();
	void GenerateObject3d();
	void GenerateParticle();

private:
	// ルートシグネチャ生成とグラフィックスパイプライン設定
	// スプライト用
	void RootSignatureGenerateSprite();
	void GraphicsPipelineGenerateSprite();
	// Object3d用
	void RootSignatureGenerateObject3d();
	void GraphicsPipelineGenerateObject3d();
	// Particle用
	void RootSignatureGenerateParticle();
	void GraphicsPipelineGenerateParticle();
private: // メンバ変数
	// ポインタ
	DirectXCommon* dxCommon_;
	// ルートシグネチャとグラフィックスパイプライン

	// スプライト
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureSprite;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSprite;

	// Object3d
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureObject3d;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateObject3d;

	// Particle
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureParticle;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateParticle;
public:
	// getter	

	// スプライト
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignatureSprite() const { return rootSignatureSprite; }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineStateSprite() const { return graphicsPipelineStateSprite; }
	// Object3d
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignatureObject3d() const { return rootSignatureObject3d; }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineStateObject3d() const { return graphicsPipelineStateObject3d; }
	// Particle
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignatureParticle() const { return rootSignatureParticle; }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineStateParticle() const { return graphicsPipelineStateParticle; }
	// setter

	// スプライト
	void SetRootSignatureSprite(Microsoft::WRL::ComPtr<ID3D12RootSignature> signature) { rootSignatureSprite = signature; }
	void SetGraphicsPipelineStateSprite(Microsoft::WRL::ComPtr<ID3D12PipelineState> pso) { graphicsPipelineStateSprite = pso; }
	// Object3d
	void SetRootSignatureObject3d(Microsoft::WRL::ComPtr<ID3D12RootSignature> signature) { rootSignatureObject3d = signature; }
	void SetGraphicsPipelineStateObject3d(Microsoft::WRL::ComPtr<ID3D12PipelineState> pso) { graphicsPipelineStateObject3d = pso; }
	// Particle
	void SetRootSignatureParticle(Microsoft::WRL::ComPtr<ID3D12RootSignature> signature) { rootSignatureParticle = signature; }
	void SetGraphicsPipelineStateParticle(Microsoft::WRL::ComPtr<ID3D12PipelineState> pso) { graphicsPipelineStateParticle = pso; }
};