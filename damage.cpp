//=============================================================================
//
// damage���� [damage.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "damage.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				SCREEN_WIDTH	// �L�����T�C�Y
#define		TEXTURE_HEIGHT				SCREEN_HEIGHT	// 
#define		TEXTURE_MAX					1				// �e�N�X�`���̐�

#define		ANIM_LENGTH					20				// �A�j���[�V�����̒���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum {
	ANIM_START,
	ANIM_ENDING,

	ANIM_NUM,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �e�N�X�`���t�@�C��
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/damage.png",
};

static BOOL		g_Load = FALSE;				// ���������s�������̃t���O

static BOOL		g_IsActive;					// ���s�t���O
static XMFLOAT3	g_Pos;						// ���W
static float	g_W;						// �T�C�Y
static float	g_H;
static float	g_Alpha;					// �A���t�@�l

static int		g_AnimStep;					// �A�j���i�K
static float	g_FrameCnt;					// ��ԗp�J�E���^

static float	g_AnimFrame[ANIM_NUM] = {	// �A�j���t���[����
	120.0f,
	300.0f,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitDamage(void)
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


	// ������
	g_IsActive = FALSE;
	g_Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_W = TEXTURE_WIDTH;
	g_H = TEXTURE_HEIGHT;
	g_Alpha = 0.0f;

	g_AnimStep = ANIM_START;
	g_FrameCnt = 0.0f;


	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitDamage(void)
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
void UpdateDamage(void) {

	if (g_IsActive != TRUE)
		return;

	switch (g_AnimStep) {

	case ANIM_START:
	{
		// �A���t�@�l���Ԃő���
		g_Alpha = g_FrameCnt / g_AnimFrame[g_AnimStep];
	}
		break;

	case ANIM_ENDING:
	{
		// �A���t�@�l���ԂŌ���
		g_Alpha = 1 - g_FrameCnt / g_AnimFrame[g_AnimStep];
	}
		break;
	}

	// �J�E���^��i�߂�
	g_FrameCnt += 0.01f;

	// ���̃X�e�b�v�ցH
	if (g_FrameCnt > g_AnimFrame[g_AnimStep]) {
		g_AnimStep++;

		// �A�j���I���H
		if (g_AnimStep == ANIM_NUM)
			g_IsActive = FALSE;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDamage(void) {

	if (g_IsActive != TRUE)
		return;

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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �ʒu��e�N�X�`�����W�𔽉f
	float px = g_Pos.x;
	float py = g_Pos.y;
	float pw = g_W;
	float ph = g_H;

	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, g_Alpha));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

//=============================================================================
// damage�G�t�F�N�g���Z�b�g
//=============================================================================
void SetDamage(void) {

	g_IsActive = TRUE;
	g_Alpha = 0.0f;

	g_AnimStep = ANIM_START;
	g_FrameCnt = 0.0f;
}