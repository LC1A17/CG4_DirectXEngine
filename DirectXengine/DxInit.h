#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <cstdlib>

#include "WinInit.h"

//DirectX汎用
class DxInit
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数
	void Initialize(WinInit* WinIni);//初期化
	void BeforeDraw();//描画前処理
	void AfterDraw();//描画後処理
	void ClearRenderTarget();//レンダーターゲットのクリア
	void ClearDepthBuffer();//深度バッファのクリア
	ID3D12Device* GetDev() { return dev.Get(); }//デバイスの取得
	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }//コマンドリストの取得

private://メンバ変数
	//初期化用
	WinInit* winini;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<ID3D12Device> dev;
	ComPtr<IDXGIAdapter1> tmpAdapter;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	D3D12_RESOURCE_BARRIER barrierDesc{};
	
private://メンバ関数
	void InitializeAdapter();//アダプタの列挙
	void InitializeDevice();//デバイスの生成
	void CreateSwapChain();//スワップチェーン
	void InitializeCmdList();//コマンドリスト
	void CreateRTV();//レンダーターゲットビュー
	void CreateDepthBuffer();//深度バッファ生成
	void CreateFence();//フェンス
};