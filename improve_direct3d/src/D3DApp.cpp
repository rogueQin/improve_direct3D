#include "D3DApp.h"
#include "MathHelper.h"

D3DApp::D3DApp(HINSTANCE hInstance, int show) : mAppInstance(hInstance), mShow(show)
{
	mWndClassName = L"AppMainWnd";
	mWndWindowName = L"D3DAppWnd";
	mAppScreenWidth = 800;
	mAppScreenHeight = 600;
}

D3DApp::~D3DApp()
{
}

bool D3DApp::Initlize() 
{
	if (!InitAppWindow())
		return false;
	if (!InitDirect3D())
		return false;
	ResetViewPort();
	return true;
}

bool D3DApp::InitAppWindow() 
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = mWndClassName;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"Register Class Failed", 0, 0);
		return false;
	}

	RECT WindoRect = { 0, 0, mAppScreenWidth, mAppScreenHeight };
	AdjustWindowRect(&WindoRect, WS_OVERLAPPEDWINDOW, false);
	int window_width = WindoRect.right - WindoRect.left;
	int window_height = WindoRect.bottom - WindoRect.top;

	mAppWnd = CreateWindow(
		mWndClassName,
		mWndWindowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_width,
		window_height,
		0,
		0,
		mAppInstance,
		0);

	SetWindowLongPtr(mAppWnd, GWLP_USERDATA, (LONG_PTR)this);

	if (mAppWnd == 0)
	{
		MessageBox(0, L"Create Window Failed", 0, 0);
		return false;
	}

	ShowWindow(mAppWnd, mShow);
	UpdateWindow(mAppWnd);

	return true;
}

LRESULT CALLBACK D3DApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	D3DApp* self = (D3DApp*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (nullptr == self)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(self->mAppWnd);
		else if (wParam == VK_LEFT)
		{
			//self->Draw();
		}
		return 0;
	case WM_SIZE:
	{
		self->OnResize(wParam, lParam, nullptr);
	}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

int D3DApp::Run()
{
	MSG msg = { 0 };
	BOOL bRet = 1;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			Update();
			Draw();
		}
	}
	return (int)msg.wParam;
}

void D3DApp::Update()
{
}

void D3DApp::Draw()
{
	ThrowIfFailed(mDirectCmdListAllocator->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAllocator.Get(), nullptr));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), 
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->RSSetViewports(1, &mViewPort);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	//mCommandList->ClearRenderTargetView(CurrentBackBufferView(), mCurrBackBuffer % 2 == 0 ? DirectX::Colors::LightSteelBlue : DirectX::Colors::LightGreen, 0, nullptr);
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	mCommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdLists[] = {mCommandList.Get()};
	mCommandQuene->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}

bool D3DApp::InitDirect3D() 
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgifactory)));
	LogAdapters();
	HRESULT hardWareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice));
	ThrowIfFailed(md3dDevice->CreateFence(mCurrentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));
	m4xMsaaQuality = msQualityLevels.NumQualityLevels;

	CreateCommondQuene();
	CreateSwapChain();
	CreateRtvAndDsvDescriptionHeaps();

	return true;
}

ID3D12Resource* D3DApp::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3DApp::LogAdapters()
{
	UINT adapter_index = 0;
	IDXGIAdapter* adapter; 
	
	while (mdxgifactory->EnumAdapters(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		mAdapterList.push_back(adapter);
		++ adapter_index;
	}

	for (size_t i = 0; i < mAdapterList.size(); ++i)
	{
		LogAdapterOutputs(mAdapterList[i]);
		ReleaseCom(mAdapterList[i]);
	}
}

void D3DApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void D3DApp::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

void D3DApp::CreateCommondQuene() 
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQuene)));
	ThrowIfFailed(md3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(mDirectCmdListAllocator.GetAddressOf())));
	ThrowIfFailed(md3dDevice->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	mCommandList->Close();
}

void D3DApp::CreateSwapChain() 
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = static_cast<UINT>(mViewPort.Width);
	sd.BufferDesc.Height = static_cast<UINT>(mViewPort.Height);
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = mAppWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(mdxgifactory->CreateSwapChain(mCommandQuene.Get(), &sd, mSwapChain.GetAddressOf()));
}

void D3DApp::CreateRtvAndDsvDescriptionHeaps() 
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void D3DApp::OnResize(WPARAM wParam, LPARAM lParam, void* reserved)
{
	mAppScreenWidth = LOWORD(lParam) > 0 ? LOWORD(lParam) : mAppScreenWidth;
	mAppScreenHeight = HIWORD(lParam) > 0 ? LOWORD(lParam) : mAppScreenHeight;

	if (md3dDevice)
		ResetViewPort();
}

void D3DApp::ResetViewPort()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAllocator);

	FlushCommandQueue();
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAllocator.Get(), nullptr));

	for (int i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mAppScreenWidth, mAppScreenHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mAppScreenWidth;
	depthStencilDesc.Height = mAppScreenHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mDsvHeap->GetCPUDescriptorHandleForHeapStart());

	mCommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQuene->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();

	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;
	mViewPort.Width = static_cast<float>(mAppScreenWidth);
	mViewPort.Height = static_cast<float>(mAppScreenHeight);
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;
	mScissorRect = { 0, 0, mAppScreenWidth, mAppScreenHeight };
}

void D3DApp::FlushCommandQueue()
{
	mCurrentFence++;
	ThrowIfFailed(mCommandQuene->Signal(mFence.Get(), mCurrentFence));
	//std::cout << "FlushCommandQueue:" << mCurrentFence << std::endl;
	UINT64 gpuFenceValue = mFence->GetCompletedValue();
	//std::cout << "gpuFenceValue:" << gpuFenceValue << std::endl;
	if (gpuFenceValue < mCurrentFence)
	{
		//std::cout << "obstruct start! gpuFenceValue:" << gpuFenceValue << std::endl;
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
		gpuFenceValue = mFence->GetCompletedValue();
		//std::cout << "obstruct end! gpuFenceValue:" << gpuFenceValue << std::endl;
	}
}

