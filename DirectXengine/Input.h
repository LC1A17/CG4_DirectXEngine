#pragma once

#include <Windows.h>
#include <wrl.h>
#include <dinput.h>
#include <DirectXMath.h>

#define DIRECTINPUT_VERSION     0x0800//DirectInputのバージョン指定

//入力クラス
class Input
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct MouseMove
	{
		LONG    lX;
		LONG    lY;
		LONG    lZ;
	};

public://静的メンバ変数
	static Input* GetInstance();

public://メンバ関数
	//初期化処理
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	//更新処理
	void Update();
	//キー判定
	bool IsKey(BYTE KeyNumber);
	//キー判定(長押し不可)
	bool IsKeyTrigger(BYTE KeyNumber);
	//キーの左ボタン押下をチェック
	bool PushMouseLeft();
	//キーの中ボタン押下をチェック
	bool PushMouseMiddle();
	//キーの左ボタントリガーをチェック
	bool TriggerMouseLeft();
	//キーの中ボタントリガーをチェック
	bool TriggerMouseMiddle();
	//マウス移動量を取得
	MouseMove GetMouseMove();

private://メンバ関数
	Input() = default;
	Input(const Input&) = delete;
	~Input() = default;
	Input& operator=(const Input&) = delete;

private://メンバ変数
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devkeyboard;
	BYTE Key[256] = {};
	BYTE OldKey[256] = {};
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};