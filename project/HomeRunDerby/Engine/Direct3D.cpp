#include "Direct3D.h"

#include <d3dcompiler.h>

#include "Texture.h"
#include "Camera.h"
#include "Global.h"


namespace Direct3D
{
	using DirectX::XMVECTOR;

	namespace
	{
		//�V�F�[�_���Ƃ̐ݒ荀��
		struct ShaderBundle
		{
			ID3D11VertexShader*		pVertexShader		= nullptr;			//���_�V�F�[�_�[
			ID3D11PixelShader*		pPixelShader		= nullptr;			//�s�N�Z���V�F�[�_�[
			ID3D11InputLayout*		pVertexLayout		= nullptr;			//���_�C���v�b�g���C�A�E�g
			ID3D11RasterizerState*	pRasterizerState	= nullptr;			//���X�^���C�U�[
		};

		////////////////////
		// �ϐ�
		ID3D11Device*				pDevice_			= nullptr;			//�f�o�C�X
		ID3D11DeviceContext*		pContext_			= nullptr;			//�f�o�C�X�R���e�L�X�g
		IDXGISwapChain*				pSwapChain_			= nullptr;			//�X���b�v�`�F�C��
		ID3D11RenderTargetView*		pRenderTargetView_	= nullptr;			//�����_�[�^�[�Q�b�g�r���[
		ID3D11Texture2D*			pDepthStencil_		= nullptr;			//�[�x�X�e���V��
		ID3D11DepthStencilView*		pDepthStencilView_	= nullptr;			//�[�x�X�e���V���r���[
		ID3D11BlendState*			pBlendState_		= nullptr;			//�u�����h�X�e�[�g
		ShaderBundle				shaderBundle_[(int)SHADER_TYPE::MAX];	//�V�F�[�_���Ƃ̐ݒ荀�ڂ��܂Ƃ߂�����
		SHADER_TYPE					currentShaderType_;						//���݂̃V�F�[�_�[�^�C�v
	}

	////////////////////
	// �֐�

	//������
	HRESULT Initialize(HWND &hWnd)
	{
		//���ݒ�,�f�o�C�X�A�R���e�L�X�g�A�X���b�v�`�F�C���̍쐬
		if (FAILED(InitAdvance(hWnd))) return E_FAIL;

		//�r���[�̏���
		if (FAILED(InitView())) return E_FAIL;

		//�V�F�[�_�[����
		if (FAILED(InitShader())) return E_FAIL;

		//�J��������
		Camera::Initialize();

		return S_OK;
	}

	//���ݒ�,�f�o�C�X�A�R���e�L�X�g�A�X���b�v�`�F�C���̍쐬
	HRESULT InitAdvance(HWND &hWnd)
	{
		//�X���b�v�`�F�C���̐ݒ�
		DXGI_SWAP_CHAIN_DESC scDesc;													//�X���b�v�`�F�C���̐ݒ荀�ڂ��܂Ƃ߂��\����
		ZeroMemory(&scDesc, sizeof(scDesc));
		scDesc.BufferDesc.Width						= Global::WINDOW_WIDTH;				//��ʕ�
		scDesc.BufferDesc.Height					= Global::WINDOW_HEIGHT;			//��ʍ���
		scDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;		//���F�g���邩
		scDesc.BufferDesc.RefreshRate.Denominator	= 1;								//�t���[�����w��@����
		scDesc.BufferDesc.RefreshRate.Numerator		= Global::GAME_FPS;					//�t���[�����w��@���q
		scDesc.Windowed								= TRUE;								//�E�B���h�E���[�h���t���X�N���[����
		scDesc.OutputWindow							= hWnd;								//�E�B���h�E�n���h��
		scDesc.BufferCount							= 1;								//�o�b�N�o�b�t�@�̖���
		scDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;	//�o�b�N�o�b�t�@�̎g��������ʂɕ`�悷�邽�߂�
		scDesc.SampleDesc.Count						= 1;								//�A���`�G�C���A�X�̐ݒ�
		scDesc.SampleDesc.Quality					= 0;								//�掿���x���i�A���`�G�C���A�X�����Ȃ�0�j

		//��L�ݒ�����ƂɃf�o�C�X�A�R���e�L�X�g�A�X���b�v�`�F�C�����쐬
		D3D_FEATURE_LEVEL level;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,						//�ǂ̃r�f�I�A�_�v�^���g�p���邩�H����Ȃ��nullptr��
			D3D_DRIVER_TYPE_HARDWARE,		//�h���C�o�̃^�C�v��n���B�ӂ���HARDWARE
			nullptr,						//��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵�Ȃ�������nullptr
			0,								//���炩�̃t���O���w�肷��B�i�f�o�b�O����D3D11_CREATE_DEVICE_DEBUG�H�j
			nullptr,						//�f�o�C�X�A�R���e�L�X�g�̃��x����ݒ�Bnullptr�ɂ��Ƃ���OK
			0,								//��̈����Ń��x�������w�肵����
			D3D11_SDK_VERSION,				//SDK�̃o�[�W�����B�K�����̒l
			&scDesc,						//�X���b�v�`�F�C���̐ݒ荀�ڂ��܂Ƃ߂��\����
			&pSwapChain_,					//��������SwapChain�̃A�h���X���Ԃ��Ă���
			&pDevice_,						//��������Device�A�h���X���Ԃ��Ă���
			&level,							//��������Device�AContext�̃��x�����Ԃ��Ă���
			&pContext_ 						//��������Context�̃A�h���X���Ԃ��Ă���
		);

		if (FAILED(hr)){
			MessageBox(nullptr, "�f�o�C�X�A�R���e�L�X�g�A�X���b�v�`�F�C���̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//�r���[�̏���
	HRESULT InitView()
	{
		{//�����_�[�^�[�Q�b�g�r���[�쐬

			//�X���b�v�`�F�[������o�b�N�o�b�t�@���擾
			ID3D11Texture2D* pBackBuffer = nullptr;

			//�o�b�N�o�b�t�@�̎擾�G���[�`�F�b�N
			if (FAILED(pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
			{
				MessageBox(nullptr, "�o�b�N�o�b�t�@�̎擾�Ɏ��s���܂���", "�G���[", MB_OK);
				return E_FAIL;
			}

			//�����_�[�^�[�Q�b�g�r���[���쐬
			if (FAILED(pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView_)))
			{
				MessageBox(nullptr, "�����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
				return E_FAIL;
			}

			//�ꎞ�I�Ƀo�b�N�o�b�t�@���擾���������Ȃ̂ŉ��
			SAFE_RELEASE(pBackBuffer);
		}

		{//�r���[�|�[�g�i�`��͈́j�ݒ�

			//�����_�����O���ʂ�\������͈�
			D3D11_VIEWPORT vp;
			vp.Width	= (float)Global::WINDOW_WIDTH;	//��
			vp.Height	= (float)Global::WINDOW_HEIGHT;	//����
			vp.MinDepth = 0.0f;							//��O
			vp.MaxDepth = 1.0f;							//��
			vp.TopLeftX = 0;							//��
			vp.TopLeftY = 0;							//��

			//�f�[�^����ʂɕ`�悷�邽�߂̈�ʂ�̐ݒ�i�p�C�v���C���j
			pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);			// �f�[�^�̓��͎�ނ��w��
			pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);			// �`����ݒ�
			pContext_->RSSetViewports(1, &vp);
			
			//�[�x�X�e���V���r���[�̐ݒ�
			D3D11_TEXTURE2D_DESC descDepth;
			descDepth.Width					= Global::WINDOW_WIDTH;		//�e�N�X�`���̕��i�e�N�Z���P�ʁj
			descDepth.Height				= Global::WINDOW_HEIGHT;	//�e�N�X�`���̍���
			descDepth.MipLevels				= 1;						//�e�N�X�`�����̃~�b�v�}�b�v���x���̍ő吔
			descDepth.ArraySize				= 1;						//�e�N�X�`���z����̃e�N�X�`���̐�
			descDepth.Format				= DXGI_FORMAT_D32_FLOAT;	//�e�N�X�`���t�H�[�}�b�g
			descDepth.SampleDesc.Count		= 1;						//�s�N�Z��������̃}���`�T���v���̐�
			descDepth.SampleDesc.Quality	= 0;						//�掿���x��
			descDepth.Usage					= D3D11_USAGE_DEFAULT;		//�e�N�X�`���̓ǂݏ������@�����ʂ���l
			descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL;	//�p�C�v���C���X�e�[�W�Ƀo�C���h���邽�߂̃t���O
			descDepth.CPUAccessFlags		= 0;						//�������CPU�A�N�Z�X�̃^�C�v���w�肷��t���O
			descDepth.MiscFlags				= 0;						//���̂��܂��ʓI�ł͂Ȃ����\�[�X�I�v�V���������ʂ���t���O
		
			//�[�x�X�e���V���r���[�p�̃e�N�X�`���̍쐬
			if (FAILED(pDevice_->CreateTexture2D(&descDepth, NULL, &pDepthStencil_)))
			{
				MessageBox(nullptr, "�[�x�X�e���V���r���[�p�̃e�N�X�`���̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
				return E_FAIL;
			}

			//�[�x�X�e���V���r���[�̍쐬
			if (FAILED(pDevice_->CreateDepthStencilView(pDepthStencil_, NULL, &pDepthStencilView_)))
			{
				MessageBox(nullptr, "�[�x�X�e���V���r���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
				return E_FAIL;
			}

			//�`����ݒ�
			pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);            
		}

		return S_OK;
	}

	//�V�F�[�_�[����
	HRESULT InitShader()
	{
		//3D�`��p
		if (FAILED(InitShader3D()))			return E_FAIL;

		//2D�`��p
		if (FAILED(InitShader2D()))			return E_FAIL;

		//�V�F�[�h���g�p���Ȃ�3D�`��p�̃V�F�[�_����
		if (FAILED(InitNoShadeShader3D()))	return E_FAIL;

		//��w�i�`��p�̃V�F�[�_����
		if (FAILED(InitShaderSky()))		return E_FAIL;

		//�n�ʕ`��p�̃V�F�[�_����
		if (FAILED(InitShaderGround()))		return E_FAIL;

		//�u�����h�X�e�[�g�̏���
		D3D11_BLEND_DESC BlendDesc;
		ZeroMemory(&BlendDesc, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable					= FALSE;						//�s�N�Z���������_�[�^�[�Q�b�g�ɐݒ肷��Ƃ��ɁA�A���t�@����J�o���b�W���}���`�T���v�����O��@�Ƃ��Ďg�p���邩
		BlendDesc.IndependentBlendEnable				= FALSE;						//���������_�[�^�[�Q�b�g�œƗ������u�����f�B���O��L���ɂ���
		BlendDesc.RenderTarget[0].BlendEnable			= TRUE;							//�u�����f�B���O���g�p���邩
		BlendDesc.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;				//�s�N�Z���V�F�[�_�[���o�͂���RGB�l�ɑ΂��Ď��s���鑀����w��
		BlendDesc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;	//�����_�[�^�[�Q�b�g�̌��݂�RGB�l�ɑ΂��Ď��s���鑀����w��
		BlendDesc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;			//����������@���`��SrcBlend��DestBlend����𐧌�
		BlendDesc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;				//�s�N�Z���V�F�[�_�[���o�͂���A���t�@�l�ɑ΂��Ď��s���鑀����w��
		BlendDesc.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;				//�����_�[�^�[�Q�b�g�̌��݂̃A���t�@�l�ɑ΂��Ď��s���鑀��
		BlendDesc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;			//�ǂ̂悤�ɑg�ݍ��킹�邩���`��SrcBlendAlpha��DestBlendAlpha����𐧌�
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//�������݃}�X�N
	
		//�u�����h�X�e�[�g�̍쐬
		if (FAILED(pDevice_->CreateBlendState(&BlendDesc, &pBlendState_)))
		{
			return E_FAIL;
		}

		//�o�̓}�[�W�X�e�[�W�̃u�����h��Ԃ�ݒ�
		const float BLEND_FACTOR[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };	//RGBA�R���|�[�l���g���Ƃ̃u�����h�t�@�N�^�[�̔z��
		pContext_->OMSetBlendState(pBlendState_, BLEND_FACTOR, 0xffffffff);

		return S_OK;
	}

	//�g�p����V�F�[�_�[�ɐݒ�
	void SetShaderBundle(SHADER_TYPE type)
	{
		currentShaderType_ = type;													//���݂̃V�F�[�_��ύX
		pContext_->VSSetShader(shaderBundle_[(int)type].pVertexShader, NULL, 0);	//���_�V�F�[�_�[
		pContext_->PSSetShader(shaderBundle_[(int)type].pPixelShader, NULL, 0);		//�s�N�Z���V�F�[�_�[
		pContext_->IASetInputLayout(shaderBundle_[(int)type].pVertexLayout);		//���_�C���v�b�g���C�A�E�g
		pContext_->RSSetState(shaderBundle_[(int)type].pRasterizerState);			//���X�^���C�U�[
	}

	//3D�V�F�[�_����
	HRESULT InitShader3D()
	{
		// ���_�V�F�[�_�̎w��i�R���p�C���j
		ID3DBlob *pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Simple3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		assert(pCompileVS != nullptr);

		// ���_�V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pVertexShader)))
		{
			MessageBox(nullptr, "Simple3D.hlsl�̒��_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		//���_�C���v�b�g���C�A�E�g�̐ݒ�
		D3D11_INPUT_ELEMENT_DESC layout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                   , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)    , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//UV���W 
					{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�@��
		};

		//���_�C���v�b�g���C�A�E�g�̍쐬
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pVertexLayout)))
		{
			MessageBox(nullptr, "Simple3D.hlsl�̒��_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);

		// �s�N�Z���V�F�[�_�w��i�R���p�C���j
		ID3DBlob *pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Simple3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
		assert(pCompilePS != nullptr);

		// �s�N�Z���V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pPixelShader)))
		{
			MessageBox(nullptr, "Simple3D.hlsl�̃s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//���X�^���C�U�̐ݒ�
		D3D11_RASTERIZER_DESC rdc	 = {};
		rdc.CullMode				 = D3D11_CULL_BACK;		//�`��ݒ�@�|���S���\��
		rdc.FillMode				 = D3D11_FILL_SOLID;	//�h��Ԃ�
		rdc.FrontCounterClockwise	 = FALSE;				//�����v���͗�

		//���X�^���C�U�쐬
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pRasterizerState)))
		{
			MessageBox(nullptr, "Simple3D.hlsl�̃��X�^���C�U�̍쐬�Ɏ��s���܂���","�G���[", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//2D�V�F�[�_����
	HRESULT InitShader2D()
	{
		// ���_�V�F�[�_�̎w��i�R���p�C���j
		ID3DBlob *pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Simple2D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		assert(pCompileVS != nullptr);
		
		// ���_�V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pVertexShader)))
		{
			MessageBox(nullptr, "���_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		//���_�C���v�b�g���C�A�E�g�̐ݒ�
		D3D11_INPUT_ELEMENT_DESC layout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0               , D3D11_INPUT_PER_VERTEX_DATA, 0},	//�ʒu
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR), D3D11_INPUT_PER_VERTEX_DATA, 0},	//UV���W 
		};

		//���_�C���v�b�g���C�A�E�g�̍쐬
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pVertexLayout)))
		{
			MessageBox(nullptr, "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);

		// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
		ID3DBlob *pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Simple2D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
		assert(pCompilePS != nullptr);
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pPixelShader)))
		{
			MessageBox(nullptr, "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//���X�^���C�U�쐬
		D3D11_RASTERIZER_DESC rdc	 = {};
		rdc.CullMode				 = D3D11_CULL_BACK;		//�`��ݒ�@�|���S���\��
		rdc.FillMode				 = D3D11_FILL_SOLID;	//�h��Ԃ�
		rdc.FrontCounterClockwise	 = FALSE;				//�����v���͗�
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pRasterizerState)))
		{
			MessageBox(nullptr, "���X�^���C�U�̍쐬�Ɏ��s���܂���",
				"�G���[", MB_OK);
			return E_FAIL;
		}
		return S_OK;
	}

	//�V�F�[�h���g�p���Ȃ�3D�`��p�̃V�F�[�_����
	HRESULT InitNoShadeShader3D()
	{
		// ���_�V�F�[�_�̎w��i�R���p�C���j
		ID3DBlob *pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/NoShade3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		assert(pCompileVS != nullptr);

		// ���_�V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pVertexShader)))
		{
			MessageBox(nullptr, "NoShade3D.hlsl�̒��_�V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		//���_�C���v�b�g���C�A�E�g�̐ݒ�
		D3D11_INPUT_ELEMENT_DESC layout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                   , D3D11_INPUT_PER_VERTEX_DATA, 0},		//�ʒu
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)    , D3D11_INPUT_PER_VERTEX_DATA, 0},		//UV���W 
					{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},		//�@��
		};

		//���_�C���v�b�g���C�A�E�g�̍쐬
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pVertexLayout)))
		{
			MessageBox(nullptr, "NoShade3D.hlsl�̒��_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);

		// �s�N�Z���V�F�[�_�w��i�R���p�C���j
		ID3DBlob *pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/NoShade3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
		assert(pCompilePS != nullptr);

		// �s�N�Z���V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pPixelShader)))
		{
			MessageBox(nullptr, "NoShade3D.hlsl�̃s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//���X�^���C�U�̐ݒ�
		D3D11_RASTERIZER_DESC rdc	= {};
		rdc.CullMode				= D3D11_CULL_BACK;		//�`��ݒ�@�|���S���\��
		rdc.FillMode				= D3D11_FILL_SOLID;		//�h��Ԃ�
		rdc.FrontCounterClockwise	= FALSE;				//�����v���͗�

		//���X�^���C�U�쐬
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pRasterizerState)))
		{
			MessageBox(nullptr, "NoShade3D.hlsl�̃��X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT Direct3D::InitShaderSky()
	{
		// ���_�V�F�[�_�̍쐬�i�R���p�C���j
		ID3DBlob* pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Sky.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pVertexShader)))
		{
			MessageBox(nullptr, "Sky.hlsl�̃s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		//���_�C���v�b�g���C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                    , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)     , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//UV���W
		};

		//���_�C���v�b�g���C�A�E�g�̍쐬
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pVertexLayout)))
		{
			MessageBox(nullptr, "Sky.hlsl�̒��_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);


		// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
		ID3DBlob* pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Sky.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);

		// �s�N�Z���V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pPixelShader)))
		{
			MessageBox(nullptr, "Sky.hlsl�̃s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//���X�^���C�U�̐ݒ�
		D3D11_RASTERIZER_DESC rdc	= {};
		rdc.CullMode				= D3D11_CULL_BACK;		//�`��ݒ�@�|���S���\��
		rdc.FillMode				= D3D11_FILL_SOLID;		//�h��Ԃ�
		rdc.FrontCounterClockwise	= FALSE;				//�����v���͗�

		//���X�^���C�U�쐬
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pRasterizerState)))
		{
			MessageBox(nullptr, "Sky.hlsl�̃��X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//�n�ʕ`��p�̃V�F�[�_����
	HRESULT InitShaderGround()
	{
		// ���_�V�F�[�_�̍쐬�i�R���p�C���j
		ID3DBlob* pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Ground.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::GROUND].pVertexShader)))
		{
			MessageBox(nullptr, "Ground.hlsl�̃s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		//���_�C���v�b�g���C�A�E�g
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                    , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ʒu
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)     , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//UV���W
			{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�@��
			{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 3 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },	//�ڐ�
		};

		//���_�C���v�b�g���C�A�E�g�̍쐬
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::GROUND].pVertexLayout)))
		{
			MessageBox(nullptr, "Ground.hlsl�̒��_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);


		// �s�N�Z���V�F�[�_�̍쐬�i�R���p�C���j
		ID3DBlob* pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Ground.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);

		// �s�N�Z���V�F�[�_�̍쐬
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::GROUND].pPixelShader)))
		{
			MessageBox(nullptr, "Ground.hlsl�̃s�N�Z���V�F�[�_�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//���X�^���C�U�̐ݒ�
		D3D11_RASTERIZER_DESC rdc	= {};
		rdc.CullMode				= D3D11_CULL_BACK;		//�`��ݒ�@�|���S���\��
		rdc.FillMode				= D3D11_FILL_SOLID;		//�h��Ԃ�
		rdc.FrontCounterClockwise	= FALSE;				//�����v���͗�

		//���X�^���C�U�쐬
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::GROUND].pRasterizerState)))
		{
			MessageBox(nullptr, "Ground.hlsl�̃��X�^���C�U�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//�`��J�n
	void BeginDraw()
	{
		//�w�i�̐F
		float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };//R,G,B,A

		//��ʂ��N���A
		pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);

		//�J����
		Camera::Update();

		//�[�x�o�b�t�@�N���A
		pContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//�`��I��
	void EndDraw()
	{
		//�X���b�v�i�o�b�N�o�b�t�@��\�ɕ\������j
		pSwapChain_->Present(0, 0);
	}

	//�������
	void Release()
	{
		for (int i = 0; i < (int)SHADER_TYPE::MAX; i++) {
			SAFE_RELEASE(shaderBundle_[i].pRasterizerState);
			SAFE_RELEASE(shaderBundle_[i].pVertexLayout);
			SAFE_RELEASE(shaderBundle_[i].pPixelShader);
			SAFE_RELEASE(shaderBundle_[i].pVertexShader);
		}

		SAFE_RELEASE(pDepthStencilView_);
		SAFE_RELEASE(pDepthStencil_);
		SAFE_RELEASE(pRenderTargetView_);
		SAFE_RELEASE(pSwapChain_);
		SAFE_RELEASE(pContext_);
		SAFE_RELEASE(pDevice_);
	}

	//�f�o�C�X���擾
	ID3D11Device * GetDevice()
	{
		 return pDevice_; 
	}

	//�f�o�C�X�R���e�L�X�g���擾
	ID3D11DeviceContext * GetContext()
	{
		return pContext_;
	}
	SHADER_TYPE GetCurrentShaderType()
	{
		return currentShaderType_;
	}
}