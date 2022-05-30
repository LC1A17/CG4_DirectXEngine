#include "WinInit.h"

const wchar_t WinInit::TitleName[] = L"LE3A_17_ニシハタ_コウキ";//ゲームタイトル
const int WinInit::WIN_WIDTH = 1280;//ウィンドウの横幅
const int WinInit::WIN_HEIGHT = 720;//ウィンドウの横幅

//ウィンドウプロシージャ
LRESULT WinInit::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY://ウィンドウが破棄された
		PostQuitMessage(0);//OSに対して、アプリの終了を伝える
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);//標準の処理を行う
}

//ウィンドウの生成
void WinInit::CreateWIN()
{
	//ウィンドウクラスの設定
	win.cbSize = sizeof(WNDCLASSEX);
	win.lpfnWndProc = (WNDPROC)WindowProc;//ウィンドウプロシージャを設定
	win.lpszClassName = TitleName;//ゲームタイトル
	win.hInstance = GetModuleHandle(nullptr);//ウィンドウハンドル
	win.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソル指定

	RegisterClassEx(&win);//ウィンドウクラスをOSに登録

	RECT wrc = { 0, 0, WIN_WIDTH, WIN_HEIGHT };//ウィンドウサイズ(X座標, Y座標, ウィンドウの横幅, ウィンドウの縦幅)
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//自動でサイズ補正

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(win.lpszClassName,	//クラス名
		TitleName,							//ゲームタイトル
		WS_OVERLAPPEDWINDOW,				//標準的なウィンドウスタイル
		CW_USEDEFAULT,						//表示X座標（OSに任せる）
		CW_USEDEFAULT,						//表示Y座標（OSに任せる）
		wrc.right - wrc.left,				//ウィンドウの横幅
		wrc.bottom - wrc.top,				//ウィンドウの縦幅
		nullptr,							//親ウィンドウハンドル
		nullptr,							//メニューハンドル
		win.hInstance,						//呼び出しアプリケーションハンドル
		nullptr);							//オプション

	ShowWindow(hwnd, SW_SHOWMAXIMIZED);//ウィンドウ表示
}

//メッセージループ
bool WinInit::MessageLoop()
{
	MSG msg{};//メッセージ

	//メッセージがある？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);//キー入力メッセージの処理
		DispatchMessage(&msg);//ウィンドウプロシージャにメッセージを送る
	}

	//終了メッセージが来たらループを抜ける
	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

//ウィンドウの破棄
void WinInit::DeleteWIN()
{
	UnregisterClass(win.lpszClassName, win.hInstance);//ウィンドウクラスを登録解除
}