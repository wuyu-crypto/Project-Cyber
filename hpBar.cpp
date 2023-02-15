//=============================================================================
//
// HP�o�[���� [hpBar.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "hpBar.h"
#include "UI.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_WIDTH				16				// �L�����T�C�Y
#define		TEXTURE_HEIGHT				80				// 
#define		TEXTURE_MAX					2				// �e�N�X�`���̐�

#define		HP_BAR_X					128				// HP�o�[����W
#define		HP_BAR_Y					32				//
#define		SLOT_INTERVAL				24				// HP�X���b�g�̊Ԋu

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
	"data/TEXTURE/HP00.png",
	"data/TEXTURE/HP01.png",
};

static BOOL		g_Load = FALSE;				// ���������s�������̃t���O
static HP_BAR	g_HPBar[PLAYER_HP_MAX];

static BOOL		g_HPBarIsActive;			// HP�o�[�L�����t���O

//=============================================================================
// ����������
//=============================================================================
HRESULT InitHPBar(void)
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


	// HP�o�[�̏�����
	g_HPBarIsActive = TRUE;

	XMFLOAT3 pos = XMFLOAT3(HP_BAR_X, HP_BAR_Y, 0.0f);
	for (int i = 0; i < PLAYER_HP_MAX; i++) {

		g_HPBar[i].pos = pos;
		g_HPBar[i].w = TEXTURE_WIDTH;
		g_HPBar[i].h = TEXTURE_HEIGHT;
		g_HPBar[i].isRemain = FALSE;

		pos = XMFLOAT3(pos.x + SLOT_INTERVAL, pos.y, pos.z);

	}

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitHPBar(void)
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
void UpdateHPBar(void) {

	// �����Ȃ���s���Ȃ�
	if (g_HPBarIsActive == FALSE) {
		return;
	}

	PLAYER* player = GetPlayer();
	int hp = player[0].hp;

	// �I�[�o�[�t���[�h�~
	if (hp < 0) {
		hp = 0;			
	}

	// �c��HP�̐������Ԃ��o�[���Z�b�g
	for (int i = 0; i < hp; i++) {
		g_HPBar[i].isRemain = TRUE;
	}

	// ������HP�̐������D�F�̃o�[���Z�b�g
	for (int i = 0; i < PLAYER_HP_MAX - hp; i++) {
		g_HPBar[i + hp].isRemain = FALSE;
	}

}

//=============================================================================
// �`�揈��
// DrawUI()�ŌĂяo��
//=============================================================================
void DrawHPBar(float alpha) {

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
	for (int i = 0; i < PLAYER_HP_MAX; i++) {

		// �ԐF���D�F�e�N�X�`����ݒ�
		if (g_HPBar[i].isRemain == TRUE) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
		}


		// �ʒu��e�N�X�`�����W�𔽉f
		float px = g_HPBar[i].pos.x - UI->pos.x;		// UI��Ǐ]
		float py = g_HPBar[i].pos.y - UI->pos.y;		//
		float pw = g_HPBar[i].w;
		float ph = g_HPBar[i].h;

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

}