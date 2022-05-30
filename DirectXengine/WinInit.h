#pragma once

#include <Windows.h>

//Windowsアプリケーション
class WinInit
{
public://静的メンバ変数
	static const wchar_t TitleName[];//ゲームタイトル
	static const int WIN_WIDTH;//ウィンドウの横幅
	static const int WIN_HEIGHT;//ウィンドウの縦幅
	
public://静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);//ウィンドウプロシージャ
	
public://メンバ関数
	void CreateWIN();//ウィンドウの生成
	void DeleteWIN();//ウィンドウ破棄
	bool MessageLoop();//メッセージループ
	HWND GetHwnd() { return hwnd; }//ウィンドウハンドルの取得
	HINSTANCE GetInstance() { return win.hInstance; };//ウィンドウクラスの取得

private://メンバ変数
	WNDCLASSEX win{};//ウィンドウクラス
	HWND hwnd = nullptr;//ウィンドウハンドル
};