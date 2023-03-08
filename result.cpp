//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "sound.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				64				// �����T�C�Y
#define TEXTURE_HEIGHT				128				//

#define TEXTURE_MAX					4				// �e�N�X�`���̐�

#define	PERIOD						128				// �_�ł������

enum {
	BG,
	GLOW,
	LOGO,
	SCORE,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result00.png",
	"data/TEXTURE/result01.png",
	"data/TEXTURE/result02.png",
	"data/TEXTURE/number384x64.png",
};

// SCORE�ϐ�
static BOOL				g_IsActive;
static float			g_w, g_h;					// ���ƍ���
static XMFLOAT3			g_Pos;						// �|���S���̍��W
static int				g_TexNo;					// �e�N�X�`���ԍ�

// RECORD�ϐ�
static int				g_ScoreRecord[SCORE_RECORD_MAX];	// RECORD
static int				g_ScoreRank;						// NEW RECORD�̏���
static XMFLOAT3			g_DefaultRecordPos;					// RECORD�̊���W
static XMFLOAT3			g_RecordDiff;						// RECORD�̊Ԋu
static XMFLOAT3			g_RecordPos[SCORE_RECORD_MAX];		// RECORD�̍��W
static float			g_RecordW;							// RECORD�̕����T�C�Y
static float			g_RecordH;							// 
static float			g_RecordStuffing;					// �����l��

static BOOL				g_Load = FALSE;

static int				g_Cnt;
static float			g_Alpha;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
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
	g_Pos   = XMFLOAT3(1178.0f, SCREEN_HEIGHT * 0.45f, 0.0f);	// SCORE�̍��W
	g_TexNo = 0;

	// RECORD�̏�����
	{
		int score[SCORE_RECORD_MAX * 2 + 1];	// �X�R�A�X���b�g
		SAVEDATA* data = GetData();

		// �܂��S�X���b�g��0������
		for (int i = 0; i < SCORE_RECORD_MAX * 2 + 1; i++) {
			score[i] = 0;
		}
		// �擪�X���b�g��RECORD������
		for (int i = 0; i < SCORE_RECORD_MAX; i++) {

			score[i] = data->score[i];
		}
		// ����SCORE������
		g_ScoreRank = SCORE_RECORD_MAX;
		score[g_ScoreRank] = GetScore();

		// �X�R�A���\�[�g
		{
			BOOL isFinish = FALSE;	// �\�[�g�I���t���O
			int find = 1;			// �Y���������l

			while (isFinish == FALSE) {

				BOOL isSwaped = FALSE;		// �\�[�g�������t���O
				for (int i = 0; i < (SCORE_RECORD_MAX * 2 + 1) - find; i++) {	// 1���ɃX���b�g��-find��\�[�g����

					if (score[i] <= score[i + 1]) {	// ���݃X���b�g�����̃X���b�g�ȉ��Ȃ�
						// �X���b�g����������
						int _score = score[i];
						score[i] = score[i + 1];
						score[i + 1] = _score;

						isSwaped = TRUE;		// �\�[�g����

						// �\�[�g�����̂�NEW RECORD�Ȃ�
						if (i + 1 == g_ScoreRank) {
							g_ScoreRank = i;		// �����L���O�X�V
						}
					}

				}

				if (isSwaped == FALSE) {	// 1����\�[�g���Ȃ������H
					isFinish = TRUE;	// �\�[�g�I��
				}

				find++;
			}
		}

		// �L���X�R�A��RECORD�ɋL�^
		for (int i = 0; i < SCORE_RECORD_MAX; i++) {
			g_ScoreRecord[i] = score[i];
		}

		// RECORD��ۑ�
		SaveData();
	}

	// RECORD���W�̏�����
	{
		g_DefaultRecordPos = XMFLOAT3(SCREEN_WIDTH * 0.67f, SCREEN_HEIGHT * 0.625f, 0.0f);
		g_RecordDiff = XMFLOAT3(SCREEN_WIDTH * 0.02f, SCREEN_HEIGHT * 0.0685f, 0.0f);	// �s��

		// ���W���v�Z
		g_RecordPos[0] = g_DefaultRecordPos;
		for (int i = 0; i < SCORE_RECORD_MAX; i++) {

			if (i > 0) {
				XMVECTOR pos = XMLoadFloat3(&g_RecordPos[i - 1]);
				XMVECTOR diff = XMLoadFloat3(&g_RecordDiff);

				XMStoreFloat3(&g_RecordPos[i], pos + diff);
			}
		}

		g_RecordW = 32;				// �����T�C�Y
		g_RecordH = 64;				//

		g_RecordStuffing = 1.0f;	// �����l��
	}


	g_Cnt = 0;

	// BGM���v���C
	PlaySound(SOUND_LABEL_BGM_RESULT);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A) || IsButtonTriggered(0, BUTTON_B) || IsButtonTriggered(0, BUTTON_X) || IsButtonTriggered(0, BUTTON_Y))
	{
		// SE���v���C
		PlaySound(SOUND_LABEL_SE_SELECT000);
		// ���[�h�`�F���W
		SetFade(FADE_OUT, MODE_TITLE);
	}



	//////////////////////////////////////////////////////////////////////////////
	// �J�E���^�Ń�����
	//////////////////////////////////////////////////////////////////////////////
	{
		g_Cnt = (g_Cnt + 1) % PERIOD;

		float ratio = (float)g_Cnt / (float)PERIOD;

		// �O���t��������( y=1-(2x-1)^2 )
		ratio = 1 -
			(2 * ratio - 1) * (2 * ratio - 1);

		g_Alpha = ratio;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
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

	// BG
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BG]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// GLOW
	{
		SetBlendState(BLEND_MODE_ADD);
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GLOW]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	// LOGO
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[LOGO]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// SCORE
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE]);

		// ��������������
		int number = GetScore();
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Pos.x + g_w * (SCORE_DIGIT - i - 1);	// �X�R�A�̕\���ʒuX(����)	// �E�̌�����`��
			float py = g_Pos.y;									// �X�R�A�̕\���ʒuY(����)	//
			float pw = g_w;										// �X�R�A�̕\����
			float ph = g_h;										// �X�R�A�̕\������

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			// ���̌���
			number /= 10;
		}

	}

	// RECORD
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE]);

		// RECORD����������
		for (int j = 0; j < SCORE_RECORD_MAX; j++) {
			int score = g_ScoreRecord[j];
			
			// ��������������
			for (int i = 0; i < SCORE_DIGIT; i++) {

				// ����\�����錅�̐���
				float x = (float)(score % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_RecordPos[j].x + g_RecordW * (SCORE_DIGIT - i - 1) * g_RecordStuffing;	// �X�R�A�̕\���ʒuX(����)	// �E�̌�����`��
				float py = g_RecordPos[j].y;								// �X�R�A�̕\���ʒuY(����)	//
				float pw = g_RecordW;										// �X�R�A�̕\����
				float ph = g_RecordH;										// �X�R�A�̕\������

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W

				// NEW RECORD�Ȃ�F�ς��ĕ\��
				if (j == g_ScoreRank) {
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
				}
				else {
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f));	// �D�F
				}

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				score /= 10;
			}
		}
	}
}

//=============================================================================
// RECORD���擾
//=============================================================================
int* GetRecord(void) {
	return &g_ScoreRecord[0];
}