#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "Model3d.h"

//3Dオブジェクト
class Object
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://構造体
	//定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		XMMATRIX mat;//3D変換行列
	};

public://静的メンバ関数
	static bool StaticInitialize(ID3D12Device* dev, int WIN_WIDTH, int WIN_HEIGHT);//静的初期化
	static void CreateGraphicsPipeline();//グラフィックパイプラインの生成
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);//描画前処理
	static void PostDraw();//描画後処理
	static Object* Create();//3Dオブジェクト生成

	//視点座標の取得
	static const XMFLOAT3& GetEye() { return eye; }

	//視点座標の設定
	static void SetEye(XMFLOAT3 eye);

	//注視点座標の取得
	static const XMFLOAT3& GetTarget() { return target; }

	//注視点座標の設定
	static void SetTarget(XMFLOAT3 target);

	//カメラ初期化処理
	static void InitializeCamera(int WIN_WIDTH, int WIN_HEIGHT);

	//ビュー行列を更新
	static void UpdateViewMatrix();

private://静的メンバ変数
	static ID3D12Device* dev;//デバイス
	static ID3D12GraphicsCommandList* cmdList;//コマンドリスト
	static ComPtr<ID3D12RootSignature> rootsignature;//ルートシグネチャ
	static ComPtr<ID3D12PipelineState> pipelinestate;//パイプラインステートオブジェクト
	static XMMATRIX matView;//ビュー行列
	static XMMATRIX matProjection;//射影行列
	static XMFLOAT3 eye;//視点座標
	static XMFLOAT3 target;//注視点座標
	static XMFLOAT3 up;//上方向ベクトル
	static XMMATRIX matBillboard;//ビルボード行列
	static XMMATRIX matBillboardY;//Y軸回りビルボード行列

public://メンバ関数
	void Initialize();//初期化処理
	void Update();//更新処理
	void Draw();//描画処理

	//座標の取得
	const XMFLOAT3& GetPosition() { return position; }

	//座標の設定
	void SetPosition(XMFLOAT3 position) { this->position = position; }

	//回転の取得
	const XMFLOAT3& GetRotation() { return rotation; }

	//回転の設定
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }

	//スケールの取得
	const XMFLOAT3& GetScale() { return scale; }

	//スケールの設定
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }

	//モデルのセット
	void SetModel(Model3d* model) { this->model3d = model; };

	//ビルボードの設定
	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }

private://メンバ変数
	ComPtr<ID3D12Resource> constBuffB0;//定数バッファ

	//色
	XMFLOAT4 color = { 1,1,1,1 };

	//ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };

	//X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };

	//ローカル座標
	XMFLOAT3 position = { 0,0,0 };

	//ローカルワールド変換行列
	XMMATRIX matWorld;

	//親オブジェクト
	Object* parent = nullptr;

	//モデル
	Model3d* model3d = nullptr;

	//ビルボード
	bool isBillboard = false;
};