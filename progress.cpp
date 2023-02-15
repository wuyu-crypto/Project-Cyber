//=============================================================================
//
// �X�e�[�W�i�s�� [progress.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "progress.h"
#include "UI.h"
#include "waveController.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				32				// �����T�C�Y
#define		TEXTURE_HEIGHT				64				// 
#define		TEXTURE_MAX					1				// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[] = {
	"data/TEXTURE/number384x64.png",
};

static BOOL						g_Load = FALSE;			// ���������s�������̃t���O


static BOOL						g_IsActive;
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Progress;					// �i�s��

//static BOOL						g_IsMax;					// �i�s�����ő�l�ɒB�����H

//=============================================================================
// ����������
//=============================================================================
HRESULT InitProgress(void)
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

	// �i�s���̏�����
	g_IsActive = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { 1040.0f, 4.0f, 0.0f };		// �\���ʒu(����)
	g_TexNo = 0;

	g_Progress = 50;

	//g_IsMax = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitProgress(void)
{
	if (g_Load == FALSE) {
		return;
	}

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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateProgress(void)
{
	// �����Ȃ���s���Ȃ�
	if (g_IsActive == FALSE) {
		return;
	}

	//if (g_IsMax == TRUE) {		// ���ɍő�l�ɒB���Ă����
	//	return;
	//}

	// ���Ԃ��擾
	float time = GetTime();
	float bossTime = GetBossTime();

	// ���������߂�
	float progress = time / bossTime;

	// 100�������Đ��������
	g_Progress = (int)(progress * 100.0f);

	// �ő�l�𒴂�����ő�l�ɌŒ�
	if (g_Progress >= PROGRESS_MAX) {
		g_Progress = PROGRESS_MAX;
		//g_IsMax = TRUE;
	}





#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// �`�揈��
// DrawUI()�ŌĂяo��
//=============================================================================
void DrawProgress(float alpha)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	UI* UI = GetUI();

	// ��������������
	int number = g_Progress;
	for (int i = 0; i < PROGRESS_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x + g_w * (PROGRESS_DIGIT - i - 1) - UI->pos.x;	// �X�R�A�̕\���ʒuX(����)	// UI��Ǐ]
		float py = g_Pos.y - UI->pos.y;									// �X�R�A�̕\���ʒuY(����)	//
		float pw = g_w;										// �X�R�A�̕\����
		float ph = g_h;										// �X�R�A�̕\������

		float tw = 1.0f / 10;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���̌���
		number /= 10;
	}
}