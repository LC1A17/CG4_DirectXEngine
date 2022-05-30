#include "WinInit.h"
#include "DxInit.h"
#include "Sound.h"
#include "GameScene.h"
#include "LightGroup.h"
#include "ParticleManager.h"
//#include "fbxsdk.h"
#include "FbxLoader.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//FbxManager* fbxManager = FbxManager::Create();

	WinInit* win = nullptr;
	DxInit* dxInit = nullptr;
	Input* input = nullptr;
	Sound* sound = nullptr;
	GameScene* gameScene = nullptr;

	//ゲームウィンドウの作成
	win = new WinInit();
	win->CreateWIN();
	//DirectX初期化処理
	dxInit = new DxInit();
	dxInit->Initialize(win);
	//入力初期化処理	
	input = Input::GetInstance();
	input->Initialize(win->GetInstance(), win->GetHwnd());
	//サウンド初期化処理
	sound = new Sound();
	sound->Initialize();
	//スプライト初期化処理
	Sprite::StaticInitialize(dxInit->GetDev(), WinInit::WIN_WIDTH, WinInit::WIN_HEIGHT);
	//ライト静的初期化
	LightGroup::StaticInitialize(dxInit->GetDev());
	//パーティクルマネージャ初期化
	ParticleManager::GetInstance()->Initialize(dxInit->GetDev());
	//FBX
	FbxLoader::GetInstance()->Initialize(dxInit->GetDev());
	//GameScene
	gameScene = new GameScene();
	gameScene->Initialize(dxInit, input, sound);

	//ゲームループ
	while (true)
	{
		//メッセージループ
		if (win->MessageLoop())
		{
			break;
		}

		//更新処理ここから
		input->Update();//Input更新
		gameScene->Update();//GameScene更新
		//更新処理ここまで

		//描画処理ここから
		dxInit->BeforeDraw();//描画開始
		gameScene->Draw();//GameScene描画
		dxInit->AfterDraw();//描画終了
		//描画処理ここまで

		//ESCで強制終了
		if (input->IsKey(DIK_ESCAPE))
		{
			break;
		}
	}

	//各種解放
	delete(gameScene);
	delete(sound);
	delete(dxInit);
	FbxLoader::GetInstance()->Finalize();

	//ウィンドウの破棄
	win->DeleteWIN();
	delete(win);

	return 0;
}