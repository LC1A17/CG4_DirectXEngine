#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>

#include "Camera.h"

//�p�[�e�B�N���}�l�[�W��
class ParticleManager
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPos
	{
		XMFLOAT3 pos;//xyz���W
		float scale;//�X�P�[��
	};

	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMMATRIX mat;//�r���[�v���W�F�N�V�����s��
		XMMATRIX matBillboard;//�r���{�[�h�s��
	};

	//�p�[�e�B�N��1��
	class Particle
	{
		//Microsoft::WRL::���ȗ�
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		//DirectX::���ȗ�
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		//���W
		XMFLOAT3 position = {};
		//���x
		XMFLOAT3 velocity = {};
		//�����x
		XMFLOAT3 accel = {};
		//�F
		XMFLOAT3 color = {};
		//�X�P�[��
		float scale = 1.0f;
		//��]
		float rotation = 0.0f;
		//�����l
		XMFLOAT3 s_color = {};
		float s_scale = 1.0f;
		float s_rotation = 0.0f;
		//�ŏI�l
		XMFLOAT3 e_color = {};
		float e_scale = 0.0f;
		float e_rotation = 0.0f;
		//���݃t���[��
		int frame = 0;
		//�I���t���[��
		int num_frame = 0;
	};

private://�萔
	static const int vertexCount = 65536;//���_��

public://�ÓI�����o�֐�
	static ParticleManager* GetInstance();

public://�����o�֐�	
	//����������
	void Initialize(ID3D12Device* dev);
	//�X�V����
	void Update();
	//�`�揈��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�J�����̃Z�b�g
	inline void SetCamera(Camera* camera) { this->camera = camera; }
	//�p�[�e�B�N���̒ǉ�
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel, float start_scale, float end_scale);
	//�f�X�N���v�^�q�[�v�̏�����
	void InitializeDescriptorHeap();
	//�O���t�B�b�N�p�C�v���C������
	void InitializeGraphicsPipeline();
	//�e�N�X�`���ǂݍ���
	void LoadTexture();
	//���f���쐬
	void CreateModel();

private://�����o�ϐ�
	//�f�o�C�X
	ID3D12Device* dev = nullptr;
	//�f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize = 0u;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	ComPtr<ID3D12PipelineState> pipelinestate;
	//�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	//�V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	//�V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//�p�[�e�B�N���z��
	std::forward_list<Particle> particles;
	//�J����
	Camera* camera = nullptr;

private:
	ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	~ParticleManager() = default;
	ParticleManager& operator=(const ParticleManager&) = delete;
};