#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//当たり判定プリミティブ

//球
struct Sphere
{
	XMVECTOR center = { 0,0,0,1 };//中心座標
	float radius = 1.0f;//半径
};

//平面
struct Plane
{
	XMVECTOR normal = { 0,1,0,0 };//法線ベクトル
	float distance = 0.0f;//原点（0, 0, 0）からの距離
};

//レイ
struct Ray
{
	XMVECTOR start = { 0,0,0,1 };//始点座標
	XMVECTOR dir = { 1,0,0,0 };//方向
};

//法線付き三角形（時計回りが表面）
class Triangle
{
public:
	//頂点座標3つ
	XMVECTOR p0;
	XMVECTOR p1;
	XMVECTOR p2;

	//法線ベクトル
	XMVECTOR normal;

	void ComputeNormal();//法線の計算
};