#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://メンバ関数
	//コンストラクタ
	PostEffect();
	//初期化処理
	void Initialize();
	//描画コマンドの実行
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//シーン描画前処理
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
	//シーン描画後処理
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
	//パイプライン生成
	void CreateGraphicsPipelineState();

public://メンバ変数
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff[2];
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//画面クリアカラー
	static const float clearColor[4];
	//グラフィックパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
};