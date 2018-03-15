#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
	m_wireFrame = nullptr;
	m_depthDisabledStencilState = nullptr;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
	bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_RASTERIZER_DESC wireFrameDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// store the vsync setting
	m_vsync_enabled = vsync;

	// create a DirectX graphics interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// use the factory to create an adapter for the primary graphics interface (video card)
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// enumerate the primary adapter ouput (monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// get the number of modes that fit for the choosen format for the adapter output (monitor)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// create a list to hold all the possible display modes for this monitor/videocard
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// now go through all the display modes and find the one that matches
	// when a match is found store the num and denom of the refresh rate
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// get the adapter (video card) description
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// store the dedicated videocard memory in Megabits
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// convert the name of the videocard to a char array
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// release displayModeList, adapterOutput, adapter, factory
	{
		// release the display mode list
		delete[] displayModeList;
		displayModeList = 0;

		// release the adapter output
		adapterOutput->Release();
		adapterOutput = 0;

		// release the adapter
		adapter->Release();
		adapter = 0;

		// release the factory
		factory->Release();
		factory = 0;
	}	

	//
	// DirectX Initialization

	// initialize swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// set to a single back buffer
	swapChainDesc.BufferCount = 1;

	// set the width and height of the back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// set regular 32-bit surface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// set the refresh rate of the back buffer
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// set the usage of the back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// set the handle for the window to render to
	swapChainDesc.OutputWindow = hwnd;

	// turn multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// set to fullscreen or windowed mode
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// set the scan line ordering and scaling to unspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// don't set the advanced flages
	swapChainDesc.Flags = 0;

	// set the feature level to DirectX 11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//
	// create the SWAP CHAIN Direct3D DEVICE and Direct3D DEVICE CONTEXT
	//   use D3D_DRIVER_TYPE_REFERENCE if no compatible videocard available
	result = D3D11CreateDeviceAndSwapChain(
		NULL,						// pAdapter 
		D3D_DRIVER_TYPE_HARDWARE,	// DriverType
		NULL,						// Software
		0,							// Flags
		&featureLevel,				// pFeatureLevels
		1,							// FeatureLevels
		D3D11_SDK_VERSION,			// SDK Version
		&swapChainDesc,				// pSwapChainDesc
		&m_swapChain,				// ppSwapChain interface
		&m_device,					// ppDevice 
		NULL,						// pFeatureLevel
		&m_deviceContext			// ppImmediateContext
		);		
	if (FAILED(result))
	{
		return false;
	}

	// get the pointer to the BACKBUFFER
	result = m_swapChain->GetBuffer(
		0,							// first buffer
		__uuidof(ID3D11Texture2D),	// ref ID to the type of interface
		(LPVOID*)&backBufferPtr		// ptr to buffer
		);
	if (FAILED(result))
	{
		return false;
	}

	//
	// create the RENDER TARGET VIEW with the backbuffer pointer
	result = m_device->CreateRenderTargetView(
		backBufferPtr,			// pResource
		NULL,					// pDesc
		&m_renderTargetView		// pRTView
		);
	if (FAILED(result))
	{
		return false;
	}

	// release pointer to the backbuffer as we no longer need it
	backBufferPtr->Release();
	backBufferPtr = 0;

	// initialize the description of the DEPTH BUFFER
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// set up the description of the DEPTH BUFFER
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

	//
	// create the texture for the DEPTH STENCIL BUFFER using the filled out description
	result = m_device->CreateTexture2D(
		&depthBufferDesc, 
		NULL, 
		&m_depthStencilBuffer
		);
	if (FAILED(result))
	{
		return false;
	}

	// initialize the description of the stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// set up the description of the stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// stencil operations if pixels is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// stencil operation if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//
	// create the DEPTH STENCIL STATE
	result = m_device->CreateDepthStencilState(
		&depthStencilDesc, 
		&m_depthStencilState
		);
	if (FAILED(result))
	{
		return false;
	}

	// set depth stencil state
	m_deviceContext->OMSetDepthStencilState(
		m_depthStencilState, 
		1
		);

	// initialize the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// set up the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//
	// create the DEPTH STENCIL VIEW
	result = m_device->CreateDepthStencilView(
		m_depthStencilBuffer, 
		&depthStencilViewDesc, 
		&m_depthStencilView
		);
	if (FAILED(result))
	{
		return false;
	}

	// BIND the RENDER TARGET VIEW and DEPTH BUFFER to the output render PIPELINE
	//  Output-Merger Stage
	m_deviceContext->OMSetRenderTargets(
		1,						// NumViews of render target views
		&m_renderTargetView,	// ppRendrTargetViews
		m_depthStencilView		// pDepthStencilView
		);

	// set up the raster description which will determine how and what polygons will be drawn
	rasterDesc.FillMode = D3D11_FILL_SOLID;			// Fillmode
	rasterDesc.CullMode = D3D11_CULL_BACK;			// Culling
	rasterDesc.FrontCounterClockwise = false;		// Front face orientation
	rasterDesc.DepthBias = 0;						// depth value offset
	rasterDesc.DepthBiasClamp = 0.f;				// maximum depth value offset
	rasterDesc.SlopeScaledDepthBias = 0.f;			// scalare on a given pixel's slope
	rasterDesc.DepthClipEnable = true;				// clipping based on distance
	rasterDesc.ScissorEnable = false;				// scissor-rectangle culling
	rasterDesc.MultisampleEnable = false;			// multisampling antialiasing
	rasterDesc.AntialiasedLineEnable = false;		// line antialiasing

	//
	// create the RASTERIZER STATE from the description we just filled out
	result = m_device->CreateRasterizerState(
		&rasterDesc, 
		&m_rasterState
		);
	if (FAILED(result))
	{
		return false;
	}

	// filling the wireframe raster description
	ZeroMemory(&wireFrameDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_NONE;
	result = m_device->CreateRasterizerState(
		&wireFrameDesc,
		&m_wireFrame
		);
	if (FAILED(result))
	{
		return false;
	}
	

	// now set the RASTERIZER STATE
	//  usually NOT done when initializig the scene
	m_deviceContext->RSSetState(m_rasterState);
	//m_deviceContext->RSSetState(m_wireFrame);

	// set up the viewport for rendering
	//  viewport tells the RS what to draw
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;

	// create the viewport
	m_deviceContext->RSSetViewports(
		1,			// nbr of viewports
		&viewport	// ptr to an array of viewports
		);

	// set up the projection matrix
	fieldOfView = 3.141592654f / 4.f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// create the projectio matrix for 3D rendering
	m_projectionMatrix = XMMatrixPerspectiveFovLH(
		fieldOfView, 
		screenAspect, 
		screenNear, 
		screenDepth
		);

	// initialize the world matrix to the identity matrix
	m_worldMatrix = XMMatrixIdentity();

	// create an orthographic projection matrix for 2D rendering
	m_orthoMatrix = XMMatrixOrthographicLH(
		(float)screenWidth, 
		(float)screenHeight, 
		screenNear, 
		screenDepth
		);

	// clear the second depth stencil state before setting the parameters
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// now create a second depth stencil state which turns off the Z buffer for 2D
	//  rendering.
	//  The only difference is that DepthEnable is set to false
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// create the state using the device
	result = m_device->CreateDepthStencilState(
		&depthDisabledStencilDesc,			// description
		&m_depthDisabledStencilState		// stencil state
		);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void D3DClass::Shutdown()
{
	// before shutting down set to windowed mode
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = nullptr;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if (m_wireFrame)
	{
		m_wireFrame->Release();
		m_wireFrame = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// set up the color to clear the buffer to
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// clear the back buffer
	m_deviceContext->ClearRenderTargetView(
		m_renderTargetView,		// pRenderTargetView
		color					// ColorRGBA
		);

	// clear the depth buffer
	m_deviceContext->ClearDepthStencilView(
		m_depthStencilView, 
		D3D11_CLEAR_DEPTH, 
		1.f, 
		0
		);

	return;
}

void D3DClass::EndScene()
{
	// present the back buffer to the screen since rendering is complete
	if (m_vsync_enabled)
	{
		// lock the screen refresh rate
		m_swapChain->Present(
			1,		// SyncInterval
			0		// Flags
			);
	}
	else
	{
		// present as fast as possible
		m_swapChain->Present(
			0,		// SyncInterval
			0		// Flags
			);
	}
	
	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(		// output merger state
		m_depthStencilState, 
		1
		);

	return;
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(
		m_depthDisabledStencilState,
		1
		);

	return;
}