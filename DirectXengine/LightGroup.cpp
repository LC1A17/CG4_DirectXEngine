#include "LightGroup.h"
#include <assert.h>

using namespace DirectX;

///静的メンバ変数の実体
ID3D12Device* LightGroup::dev = nullptr;

//初期化処理
void LightGroup::StaticInitialize(ID3D12Device* dev)
{
	//再初期化チェック
	assert(!LightGroup::dev);

	//nullptrチェック
	assert(dev);

	LightGroup::dev = dev;
}

//インスタンス生成
LightGroup* LightGroup::Create()
{
	//3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();
	//初期化処理
	instance->Initialize();

	return instance;
}

//初期化処理
void LightGroup::Initialize()
{
	//nullptrチェック
	assert(dev);

	DefaultLightSetting();

	HRESULT result = S_FALSE;
	
	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	if (FAILED(result))
	{
		assert(0);
	}

	//定数バッファへデータ転送
	TransferConstBuffer();
}

//更新処理
void LightGroup::Update()
{
	//値の更新があった時だけ定数バッファに転送する
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

//描画処理
void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff->GetGPUVirtualAddress());
}

//定数バッファ転送
void LightGroup::TransferConstBuffer()
{
	HRESULT result = S_FALSE;

	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//環境光
		constMap->ambientColor = ambientColor;
		//平行光源
		for (int i = 0; i < DirLightNum; i++)
		{
			//ライトが有効なら設定を転送
			if (dirLights[i].IsActive())
			{
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightcolor = dirLights[i].GetLightColor();
			}
			//ライトが無効ならライト色を0に
			else
			{
				constMap->dirLights[i].active = 0;
			}
		}
		//点光源
		for (int i = 0; i < PointLightNum; i++)
		{
			//ライトが有効なら設定を転送
			if (pointLights[i].IsActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor = pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights[i].GetLightAtten();
			}
			//ライトが無効ならライト色を0に
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}
		//スポットライト
		for (int i = 0; i < SpotLightNum; i++)
		{
			//ライトが有効なら設定を転送
			if (spotLights[i].IsActive())
			{
				constMap->spotLights[i].active = 1;
				constMap->spotLights[i].lightv = -spotLights[i].GetLightDir();
				constMap->spotLights[i].lightpos = spotLights[i].GetLightPos();
				constMap->spotLights[i].lightcolor = spotLights[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos = spotLights[i].GetLightFactorAngleCos();
			}
			//ライトが無効ならライト色を0に
			else
			{
				constMap->spotLights[i].active = 0;
			}
		}
		//丸影
		for (int i = 0; i < CircleShadowNum; i++)
		{
			//有効なら設定を転送
			if (circleShadows[i].IsActive())
			{
				constMap->circleShadows[i].active = 1;
				constMap->circleShadows[i].dir = -circleShadows[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos = circleShadows[i].GetFactorAngleCos();
			}
			//無効なら色を0に
			else
			{
				constMap->circleShadows[i].active = 0;
			}
		}
		constBuff->Unmap(0, nullptr);
	}
}

//標準のライト設定
void LightGroup::DefaultLightSetting()
{
	dirLights[0].SetActive(true);
	dirLights[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[0].SetLightDir({ 0.0f, -1.0f, 0.0f, 0 });

	dirLights[1].SetActive(true);
	dirLights[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[1].SetLightDir({ +0.5f, +0.1f, +0.2f, 0 });

	dirLights[2].SetActive(true);
	dirLights[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[2].SetLightDir({ -0.5f, +0.1f, -0.2f, 0 });
}

//環境光のライト色セット
void LightGroup::SetAmbientColor(const XMFLOAT3& color)
{
	ambientColor = color;
	dirty = true;
}

//平行光源の有効フラグセット
void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetActive(active);
}

//平行光源のライト方向セット
void LightGroup::SetDirLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetLightDir(lightdir);
	dirty = true;
}

//平行光源のライト色セット
void LightGroup::SetDirLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetLightColor(lightcolor);
	dirty = true;
}

//点光源の有効フラグをセット
void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetActive(active);
}

//点光源のライト座標をセット
void LightGroup::SetPointLightPos(int index, const XMFLOAT3& lightpos)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightPos(lightpos);
	dirty = true;
}

//点光源のライト色をセット
void LightGroup::SetPointLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightColor(lightcolor);
	dirty = true;
}

//点光源のライト距離減衰係数をセット
void LightGroup::SetPointLightAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

//スポットライトの有効フラグをセット
void LightGroup::SetSpotLightActive(int index, bool active)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetActive(active);
}

//スポットライトのライト方向をセット
void LightGroup::SetSpotLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightDir(lightdir);
	dirty = true;
}

//スポットライトのライト座標をセット
void LightGroup::SetSpotLightPos(int index, const XMFLOAT3& lightpos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightPos(lightpos);
	dirty = true;
}

//スポットライトのライト色をセット
void LightGroup::SetSpotLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightColor(lightcolor);
	dirty = true;
}

//スポットライトのライト距離減衰係数をセット
void LightGroup::SetSpotLightAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

//スポットライトのライト減衰角度をセット
void LightGroup::SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightFactorAngle(lightFactorAngle);
	dirty = true;
}

//丸影の有効フラグをセット
void LightGroup::SetCircleShadowActive(int index, bool active)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetActive(active);
}

//丸影のキャスター座標をセット
void LightGroup::SetCircleShadowCasterPos(int index, const XMFLOAT3& casterPos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetCasterPos(casterPos);
	dirty = true;
}

//丸影の方向をセット
void LightGroup::SetCircleShadowDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetDir(lightdir);
	dirty = true;
}

//丸影のキャスターとライトの距離をセット
void LightGroup::SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetDistanceCasterLight(distanceCasterLight);
	dirty = true;
}

//丸影の距離減衰係数をセット
void LightGroup::SetCircleShadowAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetAtten(lightAtten);
	dirty = true;
}

//丸影の減衰角度をセット
void LightGroup::SetCircleShadowFactorAngle(int index, const XMFLOAT2& lightFactorAngle)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetFactorAngle(lightFactorAngle);
	dirty = true;
}