//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "score.h"
#include "UI.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				44				// �����T�C�Y
#define		TEXTURE_HEIGHT				96				// 
#define		TEXTURE_MAX					1				// �e�N�X�`���̐�

#define		RANDOM_PLUS					0.1f			// �f�t�H���g���_���ő�ŉ��{�v���X�����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/number384x64.png",
};

static BOOL				g_Load = FALSE;				// ���������s�������̃t���O

static BOOL				g_IsActive;					// true:�g���Ă���  false:���g�p
static float			g_w, g_h;					// ���ƍ���
static XMFLOAT3			g_Pos;						// �|���S���̍��W
static int				g_TexNo;					// �e�N�X�`���ԍ�

static int				g_Score;						// �X�R�A

//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
{
	ID3D11Device *pDevice = GetDevice();

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


	// �X�R�A�̏�����
	g_IsActive   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 1472.0f, 12.0f, 0.0f };
	g_TexNo = 0;

	g_Score = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{

}

//=============================================================================
// �`�揈��
// DrawUI()�ŌĂяo��
//=============================================================================
void DrawScore(float alpha)
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
	int number = g_Score;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x + g_w * (SCORE_DIGIT - i - 1) - UI->pos.x;	// �X�R�A�̕\���ʒuX(����)	// UI��Ǐ] // �E�̌�����`��
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


//=============================================================================
// �X�R�A�����Z����(�u������)
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddScore(int add)
{
	// �f�t�H���g���_��RANBOM_PLUS�{�܂Ń����_���Ƀv���X
	add += rand() % (int)(add * RANDOM_PLUS);

	g_Score += add;
	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}

}

//=============================================================================
// �X�R�A���擾
//=============================================================================
int GetScore(void)
{
	return g_Score;
}