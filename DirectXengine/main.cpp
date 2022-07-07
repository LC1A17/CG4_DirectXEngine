#include "WinInit.h"
#include "DxInit.h"
#include "Sound.h"
#include "GameScene.h"
#include "LightGroup.h"
#include "PostEffect.h"
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
	PostEffect* postEffect = nullptr;

	//�Q�[���E�B���h�E�̍쐬
	win = new WinInit();
	win->CreateWIN();
	//DirectX����������
	dxInit = new DxInit();
	dxInit->Initialize(win);
	//���͏���������	
	input = Input::GetInstance();
	input->Initialize(win->GetInstance(), win->GetHwnd());
	//�T�E���h����������
	sound = new Sound();
	sound->Initialize();
	//�X�v���C�g����������
	Sprite::StaticInitialize(dxInit->GetDev(), WinInit::WIN_WIDTH, WinInit::WIN_HEIGHT);
	//3D�I�u�W�F�N�g
	Object::StaticInitialize(dxInit->GetDev(), WinInit::WIN_WIDTH, WinInit::WIN_HEIGHT);
	//���C�g�ÓI������
	LightGroup::StaticInitialize(dxInit->GetDev());
	//�p�[�e�B�N���}�l�[�W��������
	ParticleManager::GetInstance()->Initialize(dxInit->GetDev());
	//�|�X�g�G�t�F�N�g�p�e�N�X�`���̓ǂݍ���
	//Sprite::LoadTexture(100, L"Resources/white1x1.png");
	//�|�X�g�G�t�F�N�g�̏�����
	postEffect = new PostEffect();
	postEffect->Initialize();
	//FBX
	FbxLoader::GetInstance()->Initialize(dxInit->GetDev());
	//GameScene
	gameScene = new GameScene();
	gameScene->Initialize(dxInit, input, sound);

	//�Q�[�����[�v
	while (true)
	{
		//���b�Z�[�W���[�v
		if (win->MessageLoop())
		{
			break;
		}

		//�X�V������������
		input->Update();//Input�X�V
		gameScene->Update();//GameScene�X�V
		//�X�V���������܂�

		//�����_�[�e�N�X�`���ւ̕`��
		postEffect->PreDrawScene(dxInit->GetCmdList());
		gameScene->Draw();
		postEffect->PostDrawScene(dxInit->GetCmdList());

		//�`�揈����������
		dxInit->BeforeDraw();//�`��J�n
		postEffect->Draw(dxInit->GetCmdList());//�|�X�g�G�t�F�N�g�̕`��
		//gameScene->Draw();//GameScene�`��
		dxInit->AfterDraw();//�`��I��
		//�`�揈�������܂�

		//ESC�ŋ����I��
		if (input->IsKey(DIK_ESCAPE))
		{
			break;
		}
	}

	//�e����
	delete(gameScene);
	delete(sound);
	delete(dxInit);
	delete(postEffect);
	FbxLoader::GetInstance()->Finalize();

	//�E�B���h�E�̔j��
	win->DeleteWIN();
	delete(win);

	return 0;
}