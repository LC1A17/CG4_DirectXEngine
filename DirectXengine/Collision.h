#pragma once

#include "CollisionPrimitive.h"

using namespace DirectX;

//当たり判定ヘルパークラス
class Collision
{
public:
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, XMVECTOR* inter = nullptr);//球と平面の当たり判定
	static void ClosestPtPoint2Triangle(const XMVECTOR& point, const Triangle& triangle, XMVECTOR* closest);//点と三角形の最近接点を求める
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr);//球と法線付き三角形の当たりチェック
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, XMVECTOR* inter = nullptr);//レイと平面の当たり判定
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, XMVECTOR* inter = nullptr);//レイと法線付き三角形の当たり判定
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, XMVECTOR* inter = nullptr);//レイと球の当たり判定
};