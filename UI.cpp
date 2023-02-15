//=============================================================================
//
// UI���� [UI.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "UI.h"
#include "player.h"
#include "enemy.h"
#include "hpBar.h"
#include "score.h"
#include "icon.h"
#include "progress.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				SCREEN_WIDTH	// �L�����T�C�Y
#define		TEXTURE_HEIGHT				256				// 
#define		TEXTURE_MAX					1				// �e�N�X�`���̐�

#define		ALPHA_NORMAL				1.0f			// �ʏ펞��
#define		ALPHA_TRANSPARENT			0.3f			// ���ߎ���

#define		ANIM_LENGTH					20				// ���߃A�j���[�V�����̒���

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
	"data/TEXTURE/UI_bar.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static UI		g_UI;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
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


	// UI�̏�����
	g_UI.isActive = TRUE;
	g_UI.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// ���ォ��\��
	g_UI.w = TEXTURE_WIDTH;
	g_UI.h = TEXTURE_HEIGHT;

	g_UI.isInside = FALSE;
	g_UI.isEntering = FALSE;
	g_UI.isExiting = FALSE;

	g_UI.alpha = ALPHA_NORMAL;
	g_UI.extraRange = -64.0f;						// �ǉ����ߔ���͈�Y

	g_UI.animFrameCnt = 0;
	
	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void) {

	g_UI.isInside = IsPlayerInsideUI() || IsEnemyInsideUI();

	if (g_UI.isInside == TRUE) {
		g_UI.alpha = ALPHA_TRANSPARENT;
	}
	else {
		g_UI.alpha = ALPHA_NORMAL;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void) {

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

	// UI�͔w�i��Ǐ]���Ȃ�

	// �ʒu��e�N�X�`�����W�𔽉f
	float px = g_UI.pos.x;
	float py = g_UI.pos.y;
	float pw = g_UI.w;
	float ph = g_UI.h;
	
	float tw = 1.0f;
	float th = 1.0f;
	float tx = 0.0f;
	float ty = 0.0f;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, g_UI.alpha));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	DrawHPBar(g_UI.alpha);			// HP�o�[��`��
	DrawScore(g_UI.alpha);			// SCORE��`��
	DrawIcon(g_UI.alpha);			// ICON��`��
	DrawProgress(g_UI.alpha);		// PROGRESS��`��
}

//=============================================================================
// UI���擾
//=============================================================================
UI* GetUI(void) {
	return &g_UI;
}