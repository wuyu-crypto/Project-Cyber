//=============================================================================
//
// �A�C�e�� [item.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "item.h"
#include "collision.h"
#include "bg.h"
#include "effect.h"
#include "sound.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define			TEXTURE_MAX			ITEM_TYPE_MAX

#define			HEAL_NUM			2						// �񕜗�

#define			SPEED_RANGE			5.0f					// �A�C�e���ړ��̏㉺�̃u����

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ITEM		g_Item[ITEM_MAX];			// �o���b�g�\����

static ITEM_DATA g_ItemData[ITEM_TYPE_MAX] = {

	{
		"data/TEXTURE/heart.png",								// �e�N�X�`����
		80.0f, 80.0f,											// �L�����T�C�Y
		XMFLOAT3(-5.0f, 0.0f, 0.0f),							// �X�s�[�h
		1, 1, 1, 4,												// ������, �p�^�[����, �E�F�C�g
		80, 80,													// �����蔻��

	},

	{
		"data/TEXTURE/power.png",								// �e�N�X�`����
		80.0f, 80.0f,											// �L�����T�C�Y
		XMFLOAT3(-5.0f, 0.0f, 0.0f),							// �X�s�[�h
		1, 1, 1, 4,												// ������, �p�^�[����, �E�F�C�g
		80, 80,													// �����蔻��

	},

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_ItemData[i].textureName,
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

	for (int i = 0; i < ITEM_MAX; i++)
	{
		g_Item[i].isActive = FALSE;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitItem(void)
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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void) {

	for (int i = 0; i < ITEM_MAX; i++) {

		// �����Ȃ���s���Ȃ�
		if (g_Item[i].isActive == FALSE) {
			continue;
		}

		int type = g_Item[i].type;

		/////////////////////////////////////////////////////////////////////////////
		// �ړ�����
		/////////////////////////////////////////////////////////////////////////////
		{
			XMVECTOR xmvPos = XMLoadFloat3(&g_Item[i].pos);
			XMVECTOR xmvSpeed = XMLoadFloat3(&g_Item[i].speed);

			XMStoreFloat3(&g_Item[i].pos, xmvPos + xmvSpeed);
		}

		// �}�b�v�O�ɏo�������(�����˂��o����)
		if (g_Item[i].pos.y > SCREEN_HEIGHT + g_Item[i].h * 0.5f) {
			g_Item[i].isActive = FALSE;
		}



		/////////////////////////////////////////////////////////////////////////////
		// �����蔻��
		/////////////////////////////////////////////////////////////////////////////
		PLAYER* player = GetPlayer();

		// �v���C���[�̐�������
		for (int j = 0; j < PLAYER_MAX; j++) {

			// �����Ă�v���C���[�̂ݎ��s
			if (player[j].isActive == FALSE) {
				continue;
			}

			BOOL collision = CollisionBB(g_Item[i].pos, g_ItemData[type].colliderW, g_ItemData[type].colliderH,
				player[j].pos, player[j].colliderW, player[j].colliderH);

			// �Փ˂��Ă��Ȃ���Ύ��s���Ȃ�
			if (collision == FALSE) {
				continue;
			}
							
			// ����
			g_Item[i].isActive = FALSE;

			// �A�C�e���ɂ���ĕ���
			switch (g_Item[i].type) {

			case ITEM_TYPE_HEART:
				AddPlayerHP(j, HEAL_NUM);
				SetEffect(EFFECT_TYPE_HEAL, g_Item[i].pos, 1);
				PlaySound(SOUND_LABEL_SE_HEAL);
				break;

			case ITEM_TYPE_POWER:
				AddPlayerPower(j, 1);
				SetEffect(EFFECT_TYPE_POWERUP, g_Item[i].pos, 2);
				PlaySound(SOUND_LABEL_SE_POWER_UP);
				break;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
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

	BG* bg = GetBG();
	for (int i = 0; i < ITEM_MAX; i++) {

		// �����Ȃ���s���Ȃ�
		if (g_Item[i].isActive == FALSE) {
			continue;
		}

		int type = g_Item[i].type;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

		float px = g_Item[i].pos.x - bg->pos.x;			// BG��Ǐ]
		float py = g_Item[i].pos.y - bg->pos.y;			//
		float pw = g_Item[i].w;
		float ph = g_Item[i].h;

		float tw = 1.0f / g_ItemData[type].patternDivideX;	// �e�N�X�`���̕�
		float th = 1.0f / g_ItemData[type].patternDivideY;	// �e�N�X�`���̍���
		float tx = (float)(g_Item[i].animPattern % g_ItemData[type].patternDivideX) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Item[i].animPattern / g_ItemData[type].patternDivideX) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// �A�C�e�����Z�b�g
//=============================================================================
void SetItem(int type, XMFLOAT3 pos) {

	for (int i = 0; i < ITEM_MAX; i++) {

		// ���g�p�X���b�g������
		if (g_Item[i].isActive == TRUE) {
			continue;
		}

		g_Item[i].isActive = TRUE;
		g_Item[i].w = g_ItemData[type].w;
		g_Item[i].h = g_ItemData[type].h;

		g_Item[i].pos = pos;
		g_Item[i].type = type;

		g_Item[i].speed = g_ItemData[type].speed;
		// �㉺�u����t����
		g_Item[i].speed.y = rand() / RAND_MAX * SPEED_RANGE; 


		g_Item[i].animFrameCnt = 0;
		g_Item[i].animPattern = 0;

		g_Item[i].colliderW = g_ItemData[type].colliderW;
		g_Item[i].colliderH = g_ItemData[type].colliderH;

		// 1�Z�b�g������I��
		return;

	}
}