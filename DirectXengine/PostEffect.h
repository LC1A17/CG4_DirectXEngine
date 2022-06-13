#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�����o�֐�
	//�R���X�g���N�^
	PostEffect();
	//����������
	void Initialize();
	//�`��R�}���h�̎��s
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�V�[���`��O����
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
	//�V�[���`��㏈��
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
	//�p�C�v���C������
	void CreateGraphicsPipelineState();

public://�����o�ϐ�
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff[2];
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//��ʃN���A�J���[
	static const float clearColor[4];
	//�O���t�B�b�N�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;
};