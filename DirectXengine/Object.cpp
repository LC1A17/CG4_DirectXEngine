#include "Object.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

//静的メンバ変数の実体
ID3D12Device* Object::dev = nullptr;
ID3D12GraphicsCommandList* Object::cmdList = nullptr;
ComPtr<ID3D12RootSignature> Object::rootsignature;
ComPtr<ID3D12PipelineState> Object::pipelinestate;
XMMATRIX Object::matView{};
XMMATRIX Object::matProjection{};
XMFLOAT3 Object::eye = { 0, 0, -50.0f };
XMFLOAT3 Object::target = { 0, 0, 0 };
XMFLOAT3 Object::up = { 0, 1, 0 };
XMMATRIX Object::matBillboard = XMMatrixIdentity();
XMMATRIX Object::matBillboardY = XMMatrixIdentity();

//静的初期化
bool Object::StaticInitialize(ID3D12Device* dev, int WIN_WIDTH, int WIN_HEIGHT)
{
	//nullptrチェック
	assert(dev);

	Object::dev = dev;

	//カメラ初期化
	InitializeCamera(WIN_WIDTH, WIN_HEIGHT);

	//グラフィックパイプラインの生成
	CreateGraphicsPipeline();

	//モデルの静的初期化
	Model3d::StaticInitialize(dev);

	return true;
}

//グラフィックパイプラインの生成
void Object::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	ComPtr<ID3DBlob> vsBlob;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob;//エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile
	(
		L"Resources/shaders/ObjectVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&vsBlob, &errorBlob
	);

	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());

		errstr += "\n";

		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile
	(
		L"Resources/shaders/ObjectPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob, &errorBlob
	);

	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";

		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{//xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;//描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0レジスタ

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[3];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;

	//バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));

	if (FAILED(result))
	{
		assert(0);
	}

	gpipeline.pRootSignature = rootsignature.Get();

	//グラフィックスパイプラインの生成
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));

	if (FAILED(result))
	{
		assert(0);
	}
}

//描画前処理
void Object::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	//PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Object::cmdList == nullptr);

	//コマンドリストをセット
	Object::cmdList = cmdList;

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());

	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());

	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//描画後処理
void Object::PostDraw()
{
	//コマンドリストを解除
	Object::cmdList = nullptr;
}

//3Dオブジェクト生成
Object* Object::Create()
{
	//3Dオブジェクトのインスタンスを生成
	Object* object = new Object();

	if (object == nullptr)
	{
		return nullptr;
	}

	//スケールをセット
	float scale_val = 20;
	object->scale = { scale_val, scale_val, scale_val };

	// 初期化
	object->Initialize();

	return object;
}

//初期化処理
void Object::Initialize()
{
	//nullptrチェック
	assert(dev);

	HRESULT result = S_FALSE;

	//定数バッファの生成
	result = dev->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);
}

//更新処理
void Object::Update()
{
	HRESULT result = S_FALSE;

	XMMATRIX matScale, matRot, matTrans;

	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//ワールド行列の合成
	matWorld = XMMatrixIdentity();//変形をリセット
	matWorld *= matScale;//ワールド行列にスケーリングを反映
	matWorld *= matRot;//ワールド行列に回転を反映
	matWorld *= matTrans;//ワールド行列に平行移動を反映

	if (isBillboard)
	{
		matWorld = XMMatrixIdentity();
		matWorld *= matScale;//ワールド行列にスケーリングを反映
		matWorld *= matRot;//ワールド行列に回転を反映
		matWorld *= matBillboard;
		matWorld *= matTrans;//ワールド行列に平行移動を反映
	}

	//親オブジェクトがあれば
	if (parent != nullptr)
	{
		//親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}

	//定数バッファへデータ転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->mat = matWorld * matView * matProjection;//行列の合成
	constBuffB0->Unmap(0, nullptr);
}

//描画処理
void Object::Draw()
{
	//nullptrチェック
	assert(dev);
	assert(Object::cmdList);

	//モデルの割り当てがなければ描画しない
	if (model3d == nullptr)
	{
		return;
	}

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());

	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());

	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	model3d->Draw(cmdList);
}

//視点座標の設定
void Object::SetEye(XMFLOAT3 eye)
{
	Object::eye = eye;

	UpdateViewMatrix();
}

//注視点座標の設定
void Object::SetTarget(XMFLOAT3 target)
{
	Object::target = target;

	UpdateViewMatrix();
}

//カメラ初期化
void Object::InitializeCamera(int WIN_WIDTH, int WIN_HEIGHT)
{
	//ビュー行列の生成
	matView = XMMatrixLookAtLH
	(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up)
	);

	//透視投影による射影行列の生成
	matProjection = XMMatrixPerspectiveFovLH
	(
		XMConvertToRadians(60.0f),
		(float)WIN_WIDTH / (float)WIN_HEIGHT,
		0.1f, 1000.0f
	);
}

//ビュー行列の更新処理
void Object::UpdateViewMatrix()
{
	//視点座標
	XMVECTOR eyePosition = XMLoadFloat3(&eye);

	//注視点座標
	XMVECTOR targetPosition = XMLoadFloat3(&target);

	//(仮の)上方向
	XMVECTOR upVector = XMLoadFloat3(&up);

	//カメラZ軸(視線方向)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	//0ベクトルだと向きが定まらないので除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	//ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//カメラX軸(右方向)
	XMVECTOR cameraAxisX;

	//X軸は上方向→Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);

	//ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	//カメラY軸(上方向)
	XMVECTOR cameraAxisY;

	//Y軸はZ軸→X軸の外積で求まる
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	//カメラの回転行列
	XMMATRIX matCameraRot;

	//カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	//転置により逆行列(逆回転)を計算
	matView = XMMatrixTranspose(matCameraRot);

	//視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);

	//カメラの位置からワールド原点へのベクトル(カメラの座標系)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);//X成分
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);//Y成分
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);//Z成分

	//一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//ビュー行列に平行移動成分を設定
	matView.r[3] = translation;

#pragma region 全方向ビルボード行列の計算
	//ビルボード行列
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma endregion 全方向ビルボード行列の計算

#pragma region Y軸回りビルボード行列の計算
	//カメラX軸、Y軸、Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X軸は共通
	ybillCameraAxisX = cameraAxisX;

	//Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);

	//Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(cameraAxisX, cameraAxisY);

	//Y軸回りビルボード行列
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma endregion Y軸回りビルボード行列の計算
}