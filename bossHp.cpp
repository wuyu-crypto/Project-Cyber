//=============================================================================
//
// �{�XHP���� [bossHp.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "bossHp.h"
#include "bg.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				512.0f			// �L�����T�C�Y
#define		TEXTURE_HEIGHT				32.0f			// 
#define		TEXTURE_MAX					3				// �e�N�X�`���̐�

enum {
	HP_BG,
	HP_RED,
	HP_FRAME,

	HP_BAR_MAX,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �e�N�X�`���t�@�C��
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bossHp00.png",
	"data/TEXTURE/bossHp01.png",
	"data/TEXTURE/bossHp02.png",

};

static BOOL		g_Load = FALSE;				// ���������s�������̃t���O

static BOOL		g_IsActive;					// �L�����H

static XMFLOAT3 g_Pos;						// HP�o�[�̍��W
static XMFLOAT3	g_Diff;						// �{�X���W����̃Y��

static float	g_W;						// �Ԃ�HP�̕�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBossHp(void)
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

	g_Diff = XMFLOAT3(-300.0f, -300.0f, 0.0f);			// �{�X���W����̃Y��

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitBossHp(void)
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
void UpdateBossHp(void) {

	// �������Ȃ���s���Ȃ�
	if (g_IsActive == FALSE) {
		return;
	}

	float HP;
	XMFLOAT3 pos;

	// �{�X������
	ENEMY* enemy = GetEnemy();
	for (int i = 0; i < ENEMY_MAX; i++) {

		if (enemy[i].type == ENEMY_TYPE_BOSS) {
			// HP���擾
			HP = (float)enemy[i].hp;
			// �ʒu���擾
			pos = enemy[i].pos;
		}
	}

	// �����v�Z
	float ratio = HP / BOSS_HP_MAX;
	// �Ԃ�HP�̕���L�k
	g_W = ratio * TEXTURE_WIDTH;

	// �ʒu�𓯊�
	XMVECTOR xmvPos = XMLoadFloat3(&pos);
	XMVECTOR xmvDiff = XMLoadFloat3(&g_Diff);
	XMStoreFloat3(&g_Pos, xmvPos + xmvDiff);

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBossHp(void) {

	if (g_IsActive == FALSE) {
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

	BG* bg = GetBG();

	//////////////////////////////////////////////////////////////////////////////
	// �w�i��`��
	//////////////////////////////////////////////////////////////////////////////
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[HP_BG]);

		// �ʒu��e�N�X�`�����W�𔽉f
		float px = g_Pos.x - bg->pos.x;		// UI��Ǐ]
		float py = g_Pos.y - bg->pos.y;		//
		float pw = TEXTURE_WIDTH;
		float ph = TEXTURE_HEIGHT;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//////////////////////////////////////////////////////////////////////////////
	// �Ԃ�HP��`��
	//////////////////////////////////////////////////////////////////////////////
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[HP_RED]);

		// �ʒu��e�N�X�`�����W�𔽉f
		float px = g_Pos.x - bg->pos.x;		// BG��Ǐ]
		float py = g_Pos.y - bg->pos.y;		//
		float pw = g_W;
		float ph = TEXTURE_HEIGHT;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//////////////////////////////////////////////////////////////////////////////
	// �g��`��
	//////////////////////////////////////////////////////////////////////////////
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[HP_FRAME]);

		// �ʒu��e�N�X�`�����W�𔽉f
		float px = g_Pos.x - bg->pos.x;		// BG��Ǐ]
		float py = g_Pos.y - bg->pos.y;		//
		float pw = TEXTURE_WIDTH;
		float ph = TEXTURE_HEIGHT;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// �{�XHP��L����
//=============================================================================
void SetBossHp(void) {
	g_IsActive = TRUE;
}

//=============================================================================
// �{�XHP�𖳌���
//=============================================================================
void HideBossHp(void) {
	g_IsActive = FALSE;
}