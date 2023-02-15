//=============================================================================
//
// �Q�[�����w�i [bg.cpp]
// Author : GP11B132 33 ���D
//
//=============================================================================
#include "bg.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH * 2)		// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)			//
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�

#define	VIBRATION_FRAME_NUM			120.0f			// �U��1���̃t���[����
#define	VIBRATION_ROOP				5.0f			// �U���̃��[�v��
#define	VIBRATION_SCALE				-100.0f			// �U���̋���(�Y���̒���)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg.jpg",
};

static BOOL	g_Load = FALSE;			// ���������s�������̃t���O
static BG	g_BG;


static BOOL				g_isVibrationAllowed;		// �U���������H
static BOOL				g_isVibrating;				// �U���t���O


static float			g_VibrationFrameCnt;			// �U���p�t���[���J�E���^
static float			g_VibrationRoopCnt;				// ���[�v�J�E���^

static float			g_VibrationFrameNum;			// �U��1���̃t���[����
static float			g_VibrationRoopNum;				// �U���̃��[�v��
static float			g_VibrationScale;				// �U���̋���



//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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


	// �w�i�̏�����
	g_BG.w = TEXTURE_WIDTH;
	g_BG.h = TEXTURE_HEIGHT;
	g_BG.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.isActive = TRUE;
	g_BG.texNo = 0;

	g_BG.scrl = 0;


	// �U���̏�����
	g_isVibrationAllowed = TRUE;
	g_isVibrating = FALSE;




	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{

	//////////////////////////////////////////////////////////////////////////////
	// �U������
	//////////////////////////////////////////////////////////////////////////////

	// �U����������Ă��邩�H
	if (g_isVibrationAllowed == TRUE) {

		if (g_isVibrating == TRUE) {

			// �J�E���^��i�߂�
			g_VibrationFrameCnt++;
			if (g_VibrationFrameCnt == g_VibrationFrameNum) {
				g_VibrationFrameCnt = 0.0f;
				g_VibrationRoopCnt++;
			}

			// �U���̒��S�_���W�����߂�(���_���獶��SCALE�̒���)
			XMFLOAT3 center = XMFLOAT3(-g_VibrationScale, 0.0f, 0.0f);

			// ����̊p�x�����߂�
			float degree = (g_VibrationFrameCnt / g_VibrationFrameNum) * 360.0f;

			// ���W�A���ɕϊ�
			float rad = XMConvertToRadians(degree);

			// �U�����W�����߂�
			XMFLOAT3 pos = XMFLOAT3(
				center.x + cosf(rad) * g_VibrationScale,
				center.y + sinf(rad) * g_VibrationScale,
				0.0f
			);

			// �����_���ړ�
			g_BG.pos = pos;


			// �U���I���H
			if (g_VibrationRoopCnt == g_VibrationRoopNum) {

				// �����_�����Z�b�g
				g_BG.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

				// �U���I��
				g_isVibrating = FALSE;

			}

			// �U��������X�N���[�������s���Ȃ�
			return;

		}

	}



	//////////////////////////////////////////////////////////////////////////////
	// �X�N���[��
	//////////////////////////////////////////////////////////////////////////////
	g_BG.scrl += 0.0005f;


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		g_BG.pos.x, g_BG.pos.y, g_BG.w, g_BG.h,
		g_BG.scrl, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}


//=============================================================================
// �U���G�t�F�N�g
//=============================================================================
void SetVibration(float frame, float roop, float scale) {

	// �U����L����
	g_isVibrating = TRUE;

	// NULL�Ȃ�f�t�H���g���Z�b�g
	if (frame == NULL) {
		g_VibrationFrameNum = VIBRATION_FRAME_NUM;
	}
	else {
		g_VibrationFrameNum = frame;
	}

	if (roop == NULL) {
		g_VibrationRoopNum = VIBRATION_ROOP;
	}
	else {
		g_VibrationRoopNum = roop;
	}

	if (scale == NULL) {
		g_VibrationScale = VIBRATION_SCALE;
	}
	else {
		g_VibrationScale = scale;
	}

	// �p�����[�^�[���N���A
	g_VibrationFrameCnt = 0.0f;
	g_VibrationRoopCnt = 0.0f;
}