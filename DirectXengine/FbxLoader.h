#pragma once

#include "fbxsdk.h"
#include "Model.h"
#include <string>
#include <cassert>
#include <d3d12.h>
#include <d3dx12.h>

class FbxLoader
{
private://エイリアス
	//std::を省略
	using string = std::string;

public://定数
	//モデル格納ルートパス
	static const string baseDirectory;

public:
	//シングルトンインスタンスの取得
	static FbxLoader* GetInstance();

	//初期化処理
	void Initialize(ID3D12Device* dev);

	//後始末
	void Finalize();

	//ファイルからFBXモデル読込
	Model* LoadModelFromFile(const string& modelName);

	//再帰的にノード構成を解析
	void ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent = nullptr);

	//メッシュ読み取り
	void ParseMesh(Model* model, FbxNode* fbxNode);

	//頂点座標読み取り
	void ParseMeshVertices(Model* model, FbxMesh* fbxMesh);

	//面情報読み取り
	void ParseMeshFaces(Model* model, FbxMesh* fbxMesh);

	//マテリアル読み取り
	void ParseMaterial(Model* model, FbxNode* fbxNode);

	//スキニング情報の読み取り
	void ParseSkin(Model* model, FbxMesh* fbxMesh);

	//テクスチャ読み込み
	void LoadTexture(Model* model, const std::string& fullpath);

	//FBXの行列をXMMatrixに変換
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

	//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

	//D3D12デバイス
	ID3D12Device* dev = nullptr;
	//FBXマネージャ
	FbxManager* fbxManager = nullptr;
	//FBXインポータ
	FbxImporter* fbxImporter = nullptr;
	//テクスチャがない場合の標準テクスチャファイル
	static const string defaultTextureFileName;
};