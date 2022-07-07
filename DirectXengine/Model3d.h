#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "Mesh.h"

//モデル
class Model3d
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:
	static const std::string baseDirectory;

private://静的メンバ変数
	static ID3D12Device* dev;//デバイス
	static UINT descriptorHandleIncrementSize;//デスクリプタサイズ

public://静的メンバ関数
	static void StaticInitialize(ID3D12Device* dev);//静的初期化
	static Model3d* CreateFromOBJ(const std::string& modelname);//OBJファイルからメッシュ生成

public://メンバ関数
	~Model3d();//デストラクタ
	void Initialize(const std::string& modelname);//初期化処理
	void Draw(ID3D12GraphicsCommandList* cmdList);//描画処理

private://メンバ変数
	std::string name;//名前
	std::vector<Mesh*> meshes;//メッシュコンテナ
	std::unordered_map<std::string, Material*> materials;//マテリアルコンテナ
	Material* defaultMaterial = nullptr;//デフォルトマテリアル
	ComPtr<ID3D12DescriptorHeap> descHeap;//デスクリプタヒープ

private://メンバ関数
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);//マテリアル読み込み
	void AddMaterial(Material* material);//マテリアル登録
	void CreateDescriptorHeap();//デスクリプタヒープの生成
	void LoadTextures();//テクスチャ読み込み
};