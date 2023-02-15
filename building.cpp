//=============================================================================
//
// �r������ [building.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "building.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					BUILDING_LABEL_MAX			// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_cyber_city002.png",
};

static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BUILDING	g_Building[BUILDING_MAX];

// �X�N���[���X�s�[�h
static float g_ScrlSpeed[BUILDING_MAX] = {
	200.0f,
};

// �o���Ԋu
static int g_Wait[BUILDING_MAX] = {
	200,
};

// �e�N�X�`���T�C�Y
static float g_TexWidth[BUILDING_MAX] = {
	1024,
};
static float g_TexHeight[BUILDING_MAX] = {
	2048,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBuilding(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �w�i�̏�����
	for (int i = 0; i < BUILDING_MAX; i++) {
		g_Building[i].w = g_TexWidth[i];
		g_Building[i].h = g_TexHeight[i];
		g_Building[i].pos = XMFLOAT3(0.0f, 300.0f, 0.0f);		// �������W(Y���W����)
		g_Building[i].isActive = FALSE;
		g_Building[i].texNo = 0;

		g_Building[i].scrlSpeed = g_ScrlSpeed[i];

		g_Building[i].wait = g_Wait[i];
		g_Building[i].waitCnt = 0;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBuilding(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBuilding(void)
{
	for (int i = 0; i < BUILDING_MAX; i++) {

		g_Building[i].waitCnt++;												// �J�E���g��i�߂�

		if (g_Building[i].waitCnt > g_Building[i].wait) {						// �Ԋu�ɒB������

			g_Building[i].isActive = TRUE;									// �A�N�e�B�u��
			g_Building[i].pos.x = SCREEN_WIDTH + g_Building[i].w * 0.5f;	// ���W���Z�b�g(�E�[��蔼���o������)
			g_Building[i].pos.y = rand() % 200 + 300;

			// ���̏o���Ԋu��ݒ�
			g_Building[i].waitCnt = 0;
		}

		// ���X�N���[��
		g_Building[i].pos.x -= g_Building[i].scrlSpeed;

		// ���[�֏o�����A�N�e�B�u��
		if (g_Building[i].pos.x < -g_Building[i].w * 0.5f) {					// ���[��蔼���o������
			g_Building[i].isActive = FALSE;
		}

	}

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBuilding(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < BUILDING_MAX; i++) {

		// ��A�N�e�B�u���������΂�
		if (g_Building[i].isActive == FALSE) {
			continue;
		}

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			g_Building[i].pos.x, g_Building[i].pos.y, g_Building[i].w, g_Building[i].h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}