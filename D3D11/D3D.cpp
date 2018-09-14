////////////////////////////////////////////////////////////////////////////////
// Filename: D3D.cpp
////////////////////////////////////////////////////////////////////////////////
#include "D3D.h"

D3D::D3D()
{
	mSwapChain = nullptr;
	mDevice = nullptr;
	mDeviceContext = nullptr;
	mRenderTargetView = nullptr;
	mDepthStencilBuffer = nullptr;
	mDepthStencilView = nullptr;

	mRasterState = nullptr;

	mAlphaEnableBlendingState = nullptr;
	mAlphaDisableBlendingState = nullptr;

	mNoDepthState = nullptr;
	mDepthStencilState = nullptr;
}

D3D::D3D(const D3D& other)
{
}

D3D::~D3D()
{
}

bool D3D::Init(HWND hwnd, int screenWidth, int screenHeight, bool vsync, bool fullscreen,
	float screenDepth, float screenNear)
{
	mVsyncEnabled = vsync;

	IDXGIFactory* factory = nullptr;
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

	IDXGIAdapter* adapter = nullptr;
	// Use the factory to create an adapter for the primary graphics interface (video card).
	HR(factory->EnumAdapters(0, &adapter));

	IDXGIOutput* adapterOutput = nullptr;
	// Enumerate the primary adapter output (monitor).
	HR(adapter->EnumOutputs(0, &adapterOutput));

	UINT numModes;
	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	DXGI_MODE_DESC* displayModeList = nullptr;
	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];

	// Now fill the display mode list structures.
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

	UINT numerator, denominator;
	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (UINT i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == static_cast<unsigned int>(screenWidth))
		{
			if (displayModeList[i].Height == static_cast<unsigned int>(screenHeight))
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	HR(adapter->GetDesc(&adapterDesc));

	// Store the dedicated video card memory in megabytes.
	mVideoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength;
	// Convert the name of the video card to a character array and store it.
	int error = wcstombs_s(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
		return false;

	delete[] displayModeList;
	displayModeList = nullptr;

	ReleaseCOM(adapterOutput);
	ReleaseCOM(adapter);
	ReleaseCOM(factory);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	if (mVsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	
	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, NULL, &mDeviceContext));

	ID3D11Texture2D* backBufferPtr = nullptr;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr));

	HR(mDevice->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView));

	ReleaseCOM(backBufferPtr);

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HR(mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer));

	// Create Raster State
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HR(mDevice->CreateRasterizerState(&rasterDesc, &mRasterState));
	mDeviceContext->RSSetState(mRasterState);

	// Create Blend State
	D3D11_BLEND_DESC bsDesc;
	ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
	bsDesc.AlphaToCoverageEnable = false;
	bsDesc.IndependentBlendEnable = false;
	bsDesc.RenderTarget[0].BlendEnable = true;
	bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	HR(mDevice->CreateBlendState(&bsDesc, &mAlphaEnableBlendingState));
	bsDesc.RenderTarget[0].BlendEnable = false;
	HR(mDevice->CreateBlendState(&bsDesc, &mAlphaDisableBlendingState));
	float blendFactors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDeviceContext->OMSetBlendState(mAlphaDisableBlendingState, blendFactors, 0xffffffff);

	// Create DepthStencil State
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));

	depthStencilDesc.DepthEnable = false;
	HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mNoDepthState));
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

	// CreateDepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView));
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Create viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	mDeviceContext->RSSetViewports(1, &viewport);

	// Init world, proj, ortho matrix
	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

	float fieldOfView, screenAspect;
	fieldOfView = XM_PI / 4.0f;
	screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	XMMATRIX m = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	XMStoreFloat4x4(&mProj, m);

	m = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);
	XMStoreFloat4x4(&mOrtho, m);

	return true;
}

void D3D::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (mSwapChain)
		mSwapChain->SetFullscreenState(false, NULL);

	ReleaseCOM(mRasterState);
	ReleaseCOM(mAlphaEnableBlendingState);
	ReleaseCOM(mAlphaDisableBlendingState);
	ReleaseCOM(mDepthStencilState);

	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDeviceContext);
	ReleaseCOM(mDevice);
	ReleaseCOM(mSwapChain);
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = {red, green, blue, alpha};
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	if (mVsyncEnabled)
		mSwapChain->Present(1, 0);
	else
		mSwapChain->Present(0, 0);
}

void D3D::SetWorldMatrix(const XMMATRIX& world)
{
	XMStoreFloat4x4(&mWorld, world);
}

void D3D::SetWorldMatrix(XMFLOAT4X4 world)
{
	mWorld = world;
}

ID3D11Device* D3D::GetDevice() const
{
	return mDevice;
}

ID3D11DeviceContext* D3D::GetDeviceContext() const
{
	return mDeviceContext;
}

XMMATRIX D3D::GetProjMatrixXM() const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX D3D::GetWorldMatrixXM() const
{
	return XMLoadFloat4x4(&mWorld);
}

XMMATRIX D3D::GetOrthoMatrixXM() const
{
	return XMLoadFloat4x4(&mOrtho);
}

XMFLOAT4X4 D3D::GetWorldMatrix() const
{
	return mWorld;
}

XMFLOAT4X4 D3D::GetProjMatrix() const
{
	return mProj;
}

XMFLOAT4X4 D3D::GetOrthoMatrix() const
{
	return mOrtho;
}

void D3D::GetVideoCardInfo(char* cardName, int& memory) const
{
	strcpy_s(cardName, 128, mVideoCardDescription);
	memory = mVideoCardMemory;
}

void D3D::TurnBlendOn() const
{
	float blendFactors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDeviceContext->OMSetBlendState(mAlphaEnableBlendingState, blendFactors, 0xffffffff);
}

void D3D::TurnBlendOff() const
{
	float blendFactors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDeviceContext->OMSetBlendState(mAlphaDisableBlendingState, blendFactors, 0xffffffff);
}

void D3D::TurnDepthOn() const
{
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
}

void D3D::TurnDepthOff() const
{
	mDeviceContext->OMSetDepthStencilState(mNoDepthState, 1);
}