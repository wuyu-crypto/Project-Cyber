//=============================================================================
//
// ICON [icon.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "icon.h"
#include "UI.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				125					// �L�����T�C�Y
#define		TEXTURE_HEIGHT				125					// 
#define		TEXTURE_MAX					PLAYER_LEVEL_MAX

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �e�N�X�`���t�@�C��
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/icon00.png",
	"data/TEXTURE/icon01.png",
	"data/TEXTURE/icon02.png",
	"data/TEXTURE/icon03.png",
	"data/TEXTURE/icon04.png",
};

static	BOOL		g_Load = FALSE;				// ���������s�������̃t���O
static	ICON		g_Icon;

static	int			g_Level;					// �v���C���[���x��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitIcon(void)
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
	g_Icon.isActive = TRUE;
	g_Icon.w = TEXTURE_WIDTH;
	g_Icon.h = TEXTURE_HEIGHT;
	g_Icon.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitIcon(void)
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
void UpdateIcon(void) {

	// �����Ȃ���s���Ȃ�
	if (g_Icon.isActive == FALSE) {
		return;
	}

	PLAYER* player = GetPlayer();
	g_Level = player[0].level;

}

//=============================================================================
// �`�揈��
// DrawUI()�ŌĂяo��
//=============================================================================
void DrawIcon(float alpha) {

	// �����Ȃ���s���Ȃ�
	if (g_Icon.isActive == FALSE) {
		return;
	}

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

	UI* UI = GetUI();
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Level]);

	float px = g_Icon.pos.x - UI->pos.x;		// UI��Ǐ]
	float py = g_Icon.pos.y - UI->pos.y;		//
	float pw = g_Icon.w;
	float ph = g_Icon.h;

	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}