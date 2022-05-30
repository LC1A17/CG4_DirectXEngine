#pragma once

#include "Camera.h"
#include "Input.h"

//デバッグ用カメラ
class DebugCamera : public Camera
{
protected://エイリアス
	//DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;
public://メンバ関数
	//コンストラクタ
	DebugCamera(int WIN_WIDTH, int WIN_HEIGHT, Input* input);

	//更新処理
	void Update() override;

	void SetDistance(float distance) { this->distance = distance; viewDirty = true; }

private:
	//入力クラスのポインタ
	Input* input;
	//カメラ注視点までの距離
	float distance = 20;
	//スケーリング
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	//回転行列
	XMMATRIX matRot = DirectX::XMMatrixIdentity();
};