#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance()
{
	static Input instance;

	return &instance;
}

//初期化処理
void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;

	assert(!dinput);

	//DirectInputオブジェクトの生成
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//キーボードデバイスの生成	
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	//マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);

	//入力データ形式のセット
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式

	//排他制御レベルのセット
	result = devkeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//入力データ形式のセット
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式

	//排他制御レベルのセット
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

//更新処理
void Input::Update()
{
	HRESULT result = S_FALSE;

	//キーボード
	//キーボード情報の取得開始
	result = devkeyboard->Acquire();
	//前フレームのキー入力をコピー
	memcpy(OldKey, Key, sizeof(Key));
	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(Key), Key);

	//マウス
	//マウス動作開始
	result = devMouse->Acquire();
	//前回の入力を保存
	mouseStatePre = mouseState;
	//マウスの入力
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
}

//キー判定
bool Input::IsKey(BYTE KeyNumber)
{
	//指定キーを押していればtrueを返す
	if (Key[KeyNumber])
	{
		return true;
	}

	//そうでなければfalseを返す
	return false;
}

//キー判定（長押し不可）
bool Input::IsKeyTrigger(BYTE KeyNumber)
{
	//前フレームがfalseで指定キーを押していればtrueを返す
	if (!OldKey[KeyNumber] && Key[KeyNumber])
	{
		return true;
	}

	//そうでなければfalseを返す
	return false;
}

//キーの左ボタン押下をチェック
bool Input::PushMouseLeft()
{
	//0でなければ押している
	if (mouseState.rgbButtons[0])
	{
		return true;
	}

	//押していない
	return false;
}

//キーの中ボタン押下をチェック
bool Input::PushMouseMiddle()
{
	//0でなければ押している
	if (mouseState.rgbButtons[2])
	{
		return true;
	}

	//押していない
	return false;
}

//キーの左ボタントリガーをチェック
bool Input::TriggerMouseLeft()
{
	//前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0])
	{
		return true;
	}

	//トリガーでない
	return false;
}

//キーの中ボタントリガーをチェック
bool Input::TriggerMouseMiddle()
{
	//前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[2] && mouseState.rgbButtons[2])
	{
		return true;
	}

	//トリガーでない
	return false;
}

//マウス移動量を取得
Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState.lX;
	tmp.lY = mouseState.lY;
	tmp.lZ = mouseState.lZ;
	return tmp;
}