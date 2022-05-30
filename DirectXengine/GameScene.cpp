#include "GameScene.h"
#include "Model.h"
#include "Collision.h"
#include "FbxLoader.h"

#include <cassert>
#include <sstream>
#include <iomanip>

#include <cstdlib>
#include <ctime>
#include <math.h>

using namespace Microsoft;
using namespace std;
using namespace DirectX;

//コンストラクタ
GameScene::GameScene()
{

}

//デストラクタ
GameScene::~GameScene()
{
	delete(back);
	delete(lightGroup);
	delete(model);
	delete(object);
}

//初期化処理
void GameScene::Initialize(DxInit* dxInit, Input* input, Sound* sound)
{
	//nullptrチェック
	assert(dxInit);
	assert(input);
	assert(sound);

	this->dxInit = dxInit;
	this->input = input;
	this->sound = sound;

	//カメラ生成
	camera = new DebugCamera(WinInit::WIN_WIDTH, WinInit::WIN_HEIGHT, input);

	//デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png"))
	{
		assert(0);
		return;
	}

	//デバッグテキスト初期化
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);
	//デバイスをセット
	Object3d::SetDevice(dxInit->GetDev());
	//カメラをセット
	Object3d::SetCamera(camera);
	//グラフィックスパイプライン生成
	Object3d::CreateGraphicsPipeline();

	//ここから追加
	srand(time(NULL));//乱数初期化

	//スプライト読み込み
	if (!Sprite::LoadTexture(1, L"Resources/image/back.png"))
	{
		assert(0);
	}

	//背景スプライト生成
	back = Sprite::Create(1, { 0.0f,0.0f });
	back->SetSize({ 1280, 720 });
	back->SetPosition({ 0.0f, 0.0f });

	//モデル名を指定してファイル読み込み
	model = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//3Dオブジェクト生成とモデルのセット
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

	//パーティクルマネージャ生成
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	//テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/tex1.png");

	//ライト生成
	lightGroup = LightGroup::Create();

	//カメラ注視点をセット
	camera->SetTarget({ 0, 2.5f, 0 });
	camera->SetDistance(8.0f);
	object->SetRotation({ 0, 90, 0 });
	object->PlayAnimation();
}

//更新処理
void GameScene::Update()
{
	object->Update();//オブジェクト更新
	lightGroup->Update();//ライト更新
	camera->Update();//カメラ更新
	particleMan->Update();//パーティクル更新
}

//描画処理
void GameScene::Draw()
{
	//コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxInit->GetCmdList();

	Sprite::PreDraw(cmdList);//スプライト描画前処理
	//ここに追加
	back->Draw();//背景
	Sprite::PostDraw();//スプライト描画後処理
	dxInit->ClearDepthBuffer();//深度バッファクリア

	object->Draw(cmdList);
	//パーティクルの描画
	particleMan->Draw(cmdList);

	Sprite::PreDraw(cmdList);//スプライト描画前処理
	//ここに追加
	//デバッグテキストの描画
	debugText->DrawAll(cmdList);
	Sprite::PostDraw();//スプライト描画後処理
}