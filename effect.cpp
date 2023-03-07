//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "effect.h"
#include "bg.h"
#include "player.h"
#include "sound.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define		TEXTURE_MAX					EFFECT_TYPE_MAX
#define		WAIT_DEFAULT				1						// �f�t�H���g�E�F�C�g�l

#define		EFFECT_MODE_GLOW_NUM		1						// GLOW���[�h�̉��Z��

#define		MANUAL_EFFECT_HEIGHT		0.95f					// ��������G�t�F�N�g�̍��W(�p�[�Z���e�[�W�\�L)

// �G�t�F�N�g�`�惂�[�h�ԍ�
enum {

	DRAW_DEFAULT,			// �f�t�H���g
	DRAW_GLOW,				// �d�ˉ��Z

	DRAW_MAX,
};

// �G�t�F�N�g�Ǐ]���x��
enum {

	EFFECT_TRACK_NONE,				// �N���Ǐ]���Ȃ�
	EFFECT_TRACK_PLAYER,			// �v���C���[��Ǐ]

	EFFECT_TRACK_MAX,
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �G�t�F�N�g�f�[�^
static EFFECT_DATA g_EffectData[EFFECT_TYPE_MAX] = {

	{ 
		"data/TEXTURE/player_normal_shot.png",				// �e�N�X�`����, 
		XMINT2(200, 200), DRAW_GLOW, 0.7f,					// �T�C�Y, �`�惂�[�h, ��
		EFFECT_TRACK_PLAYER, XMFLOAT3(90.0f, 0.0f, 0.0f),	// �Ǐ]���x��, �Y��
		TRUE,												// �r���t���O
		XMINT2(2, 2), 3, WAIT_DEFAULT,						// ������, �p�^�[����, �E�F�C�g,
		EFFECT_LAYER_FRONT_OF_PLAYER,						// ���C���[
	},

	{
		"data/TEXTURE/hit00.png",
		XMINT2(200, 200), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	{
		"data/TEXTURE/hit01.png",
		XMINT2(200, 200), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 2,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},


	{ 
		"data/TEXTURE/bomb.png", 
		XMINT2(200, 200), DRAW_DEFAULT, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 16, WAIT_DEFAULT,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	{
		"data/TEXTURE/backfire.png",
		XMINT2(100, 100), DRAW_DEFAULT, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(-125.0f, -5.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},
		
	{
		"data/TEXTURE/backglow00.png",
		XMINT2(300, 300), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(-25.0f, 5.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 8,
		EFFECT_LAYER_BEHIND_PLAYER,
	},
	
	{
		"data/TEXTURE/backparticles.png",
		XMINT2(400, 400), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(-100.0f, -5.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 16, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},

	{
		"data/TEXTURE/flash00.png",
		XMINT2(200, 200), DRAW_GLOW, 0.5f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	{
		"data/TEXTURE/heal.png",
		XMINT2(300, 300), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},
	
	{
		"data/TEXTURE/powerup.png",
		XMINT2(400, 400), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 10, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},

	{
		"data/TEXTURE/popped00.png",
		XMINT2(200, 200), DRAW_GLOW, 1.0f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 4,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
		
	{
		"data/TEXTURE/popped01.png",
		XMINT2(200, 200), DRAW_GLOW, 0.8f,
		EFFECT_TRACK_PLAYER, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(2, 1), 2, 4,
		EFFECT_LAYER_BEHIND_PLAYER,
	},


	{	// �{�X��������(���ʂ̔������f�J��)
		"data/TEXTURE/bomb.png",
		XMINT2(500, 500), DRAW_DEFAULT, 1.0f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(4, 4), 16, WAIT_DEFAULT,
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},

	// ��������G�t�F�N�g
	{
		"data/TEXTURE/manualEffect00.png",
		XMINT2(1024, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 1,		// ���[�v
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
	{
		"data/TEXTURE/manualEffect01.png",
		XMINT2(1024, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 60,	// �\���b��
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
	{
		"data/TEXTURE/manualEffect02.png",
		XMINT2(256, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 1,		// ���[�v
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},
	{
		"data/TEXTURE/manualEffect03.png",
		XMINT2(512, 128), DRAW_DEFAULT, 0.5f,
		EFFECT_TRACK_NONE, XMFLOAT3(0.0f, 0.0f, 0.0f),
		FALSE,
		XMINT2(1, 1), 1, 1,		// ���[�v
		EFFECT_LAYER_FRONT_OF_PLAYER,
	},


};

static BOOL		g_Load = FALSE;										// ���������s�������̃t���O
static EFFECT	g_Effect[EFFECT_MAX];								// �G�t�F�N�g�\����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++) {

		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_EffectData[i].textureName,
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

	// ����������
	for (int i = 0; i < EFFECT_MAX; i++) {
		g_Effect[i].isActive = FALSE;	// �@�S�G�t�F�N�g������
	}

	// ���[�h�ɂ��G�t�F�N�g���Z�b�g
	if (GetMode() == MODE_TITLE) {

		// �p�b�h�L�����Ȃ炵�Ȃ�
		if (!IsPadActive()) {
			// ����������Z�b�g
			SetEffect(EFFECT_TYPE_MANUAL00, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * MANUAL_EFFECT_HEIGHT, 0.0f), EFFECT_LOOP_FOREVER);
		}

	}
	else if (GetMode() == MODE_GAME) {
		// BACKFIRE�G�t�F�N�g��L����(0�Ԗڃv���C���[�ɑ΂���)
		SetEffect(EFFECT_TYPE_PLAYER_BACKFIRE, 0, EFFECT_LOOP_FOREVER);
		// BACKGLOW�G�t�F�N�g��L����(0�Ԗڃv���C���[�ɑ΂���)
		SetEffect(EFFECT_TYPE_PLAYER_BACKGLOW, 0, EFFECT_LOOP_FOREVER);
		// BACKPARTICLES��L����
		SetEffect(EFFECT_TYPE_PLAYER_BACKPARTICLES, 0, EFFECT_LOOP_FOREVER);

		// �p�b�h�L�����Ȃ炵�Ȃ�
		if (!IsPadActive()) {
			// ����������Z�b�g
			SetEffect(EFFECT_TYPE_MANUAL01, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * MANUAL_EFFECT_HEIGHT, 0.0f), 30.0f);	// 30�b�ԃv���C
		}
	}
	else if (GetMode() == MODE_GAMEOVER || GetMode() == MODE_RESULT) {

		// �p�b�h�L�����Ȃ炵�Ȃ�
		if (!IsPadActive()) {

			// ����������Z�b�g
			SetEffect(EFFECT_TYPE_MANUAL02, XMFLOAT3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * MANUAL_EFFECT_HEIGHT, 0.0f), EFFECT_LOOP_FOREVER);
		}

	}



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
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

	// �S�G�t�F�N�g�𖳌���
	for (int i = 0; i < EFFECT_MAX; i++) {
		g_Effect[i].isActive = FALSE;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEffect(void)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (g_Effect[i].isActive == TRUE)
		{
			int type = g_Effect[i].type;

			////////////////////////////////////////////////////////////////////////////
			// �A�j���[�V��������
			////////////////////////////////////////////////////////////////////////////
			g_Effect[i].animFrameCnt++;
			if ((g_Effect[i].animFrameCnt % g_EffectData[type].wait) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Effect[i].animPattern = (g_Effect[i].animPattern + 1) % g_EffectData[type].patternNum;
			}

			// �������[�v�Ȃ�I�����肵�Ȃ�
			if (g_Effect[i].roop != EFFECT_LOOP_FOREVER) {

				// �A�j���[�V�����I���H
				if (g_Effect[i].animFrameCnt >= g_Effect[i].animTotalFrame) {
					g_Effect[i].isActive = FALSE;
					continue;
				}

			}



			////////////////////////////////////////////////////////////////////////////
			// �G�t�F�N�g�Ǐ]����
			////////////////////////////////////////////////////////////////////////////
			
			// �v���C���[��Ǐ]
			if (g_EffectData[type].trackLabel == EFFECT_TRACK_PLAYER) {

				PLAYER* player = GetPlayer() + g_Effect[i].trackTarget;

				// �v���C���[�������ł����
				if (player->isActive == TRUE) {

					XMVECTOR xmvPos = XMLoadFloat3(&player->pos);
					XMVECTOR xmvDiff = XMLoadFloat3(&g_EffectData[type].diff);

					// �Ǐ]
					XMStoreFloat3(&g_Effect[i].pos, xmvPos + xmvDiff);
				}
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(int layer)
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
	for (int i = 0; i < TEXTURE_MAX; i++) {

		if (g_Effect[i].isActive == TRUE) {

			int type = g_Effect[i].type;

			// ���C���[�����Ă��Ȃ���Ε`�悵�Ȃ�
			if (g_EffectData[type].layer != layer) {
				continue;
			}

			////////////////////////////////////////////////////////////////////////////
			// DEFAULT
			////////////////////////////////////////////////////////////////////////////
			if (g_EffectData[type].mode == DRAW_DEFAULT) {

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

				float px = g_Effect[i].pos.x - bg->pos.x;			// BG��Ǐ]
				float py = g_Effect[i].pos.y - bg->pos.y;			//
				float pw = g_Effect[i].w;
				float ph = g_Effect[i].h;

				float tw = 1.0f / g_EffectData[type].patternDivide.x;
				float th = 1.0f / g_EffectData[type].patternDivide.y;
				float tx = (float)(g_Effect[i].animPattern % g_EffectData[type].patternDivide.x) * tw;
				float ty = (float)(g_Effect[i].animPattern / g_EffectData[type].patternDivide.x) * th;

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_EffectData[type].alpha));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			////////////////////////////////////////////////////////////////////////////
			// GLOW
			////////////////////////////////////////////////////////////////////////////
			else if (g_EffectData[type].mode == DRAW_GLOW) {

				SetBlendState(BLEND_MODE_ADD);

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

				float px = g_Effect[i].pos.x - bg->pos.x;			// BG��Ǐ]
				float py = g_Effect[i].pos.y - bg->pos.y;			//
				float pw = g_Effect[i].w;
				float ph = g_Effect[i].h;

				float tw = 1.0f / g_EffectData[type].patternDivide.x;
				float th = 1.0f / g_EffectData[type].patternDivide.y;
				float tx = (float)(g_Effect[i].animPattern % g_EffectData[type].patternDivide.x) * tw;
				float ty = (float)(g_Effect[i].animPattern / g_EffectData[type].patternDivide.x) * th;

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, g_EffectData[type].alpha));

				for (int i = 0; i < EFFECT_MODE_GLOW_NUM; i++) {

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);
				}

				SetBlendState(BLEND_MODE_ALPHABLEND);
			}
		}
	}
}

//=============================================================================
// �G�t�F�N�g���擾
//=============================================================================
EFFECT* GetEffect(void) {
	return &g_Effect[0];
}

//=============================================================================
// �G�t�F�N�g�f�[�^���擾
//=============================================================================
EFFECT_DATA* GetEffectData(void) {
	return &g_EffectData[0];
}

//=============================================================================
// �Ǐ]�G�t�F�N�g���Z�b�g
//=============================================================================
void SetEffect(int type, int target, int loop) {

	for (int i = 0; i < EFFECT_MAX; i++) {

		if (g_Effect[i].isActive == TRUE) {
			continue;
		}

		// �r���t���O�H
		if (g_EffectData[type].isOnly == TRUE) {
			StopEffect(type);		// ���������ׂĖ���
		}

		g_Effect[i].isActive = TRUE;
		g_Effect[i].w = g_EffectData[type].size.x;
		g_Effect[i].h = g_EffectData[type].size.y;
		g_Effect[i].type = type;
		g_Effect[i].roop = loop;

		g_Effect[i].animFrameCnt = 0;
		g_Effect[i].animPattern = 0;
		g_Effect[i].animTotalFrame = loop * g_EffectData[type].patternNum * g_EffectData[type].wait;

		g_Effect[i].trackTarget = target;

		return;			// 1�Z�b�g�ł�����I��
	}
}

//=============================================================================
// �G�t�F�N�g���Z�b�g
//=============================================================================
void SetEffect(int type, XMFLOAT3 pos, int loop) {

	for (int i = 0; i < EFFECT_MAX; i++) {

		// ���g�p�X���b�g������
		if (g_Effect[i].isActive == TRUE) {
			continue;
		}

		// �r���t���O�H
		if (g_EffectData[type].isOnly == TRUE) {
			StopEffect(type);		// ���������ׂĖ���
		}

		g_Effect[i].isActive = TRUE;
		g_Effect[i].w = g_EffectData[type].size.x;
		g_Effect[i].h = g_EffectData[type].size.y;
		g_Effect[i].pos = pos;
		g_Effect[i].type = type;
		g_Effect[i].roop = loop;

		g_Effect[i].animFrameCnt = 0;
		g_Effect[i].animPattern = 0;
		g_Effect[i].animTotalFrame = loop * g_EffectData[type].patternNum * g_EffectData[type].wait;

		return;			// 1�Z�b�g�ł�����I��
	}
}

//=============================================================================
// �G�t�F�N�g�𒆎~
//=============================================================================
void StopEffect(int type) {

	for (int i = 0; i < EFFECT_MAX; i++) {

		// ����G�t�F�N�g�����ׂĖ�����
		if (g_Effect[i].type == type) {
			g_Effect[i].isActive = FALSE;
		}

	}
}