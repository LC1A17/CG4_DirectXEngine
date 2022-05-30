#pragma once

#include "DxInit.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ParticleManager.h"
#include "DebugText.h"
#include "Sound.h"
#include "DebugCamera.h"
#include "CollisionPrimitive.h"
#include "LightGroup.h"
#include <vector>

//ゲームシーン
class GameScene
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス
	//ループ分岐用
	enum Scene
	{
		Title, Game, Clear, GameOver
	};

private://静的メンバ変数
	static const int debugTextTexNumber = 0;

public://メンバ関数
	//コンストラクタ
	GameScene();
	//デストラクタ
	~GameScene();
	//初期化処理
	void Initialize(DxInit* dxInit, Input* input, Sound* sound);
	//更新処理
	void Update();
	//描画処理
	void Draw();

private://メンバ変数
	DxInit* dxInit = nullptr;
	Input* input = nullptr;
	Sound* sound = nullptr;
	DebugText* debugText;

	Model* model = nullptr;
	Object3d* object = nullptr;
	DebugCamera* camera = nullptr;
	ParticleManager* particleMan = nullptr;
	LightGroup* lightGroup = nullptr;

	Sprite* back = nullptr;//背景
};