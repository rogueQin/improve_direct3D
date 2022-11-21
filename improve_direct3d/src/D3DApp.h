#ifndef _D3DApp_H
#define _D3DApp_H

#include "D3DUtil.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance, int show);
	~D3DApp();
	virtual bool Initlize();
	virtual int Run();

protected:
	virtual void Update();
	virtual void Draw();


	virtual void OnResize(WPARAM wParam, LPARAM lParam, void*reserved = nullptr);
	virtual void ResetViewPort();
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam, void*reserved = nullptr);


protected:
	bool InitAppWindow();
	bool InitDirect3D();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	void CreateCommondQuene();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptionHeaps();

	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void FlushCommandQueue();

protected:
	int mShow;
	HINSTANCE mAppInstance;
	LPCWSTR mWndClassName;
	LPCWSTR mWndWindowName;
	HWND mAppWnd;

	Microsoft::WRL::ComPtr <IDXGIFactory4> mdxgifactory;
	Microsoft::WRL::ComPtr <ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr <ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;
	std::vector<IDXGIAdapter*> mAdapterList;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;

	UINT mRtvDescriptorSize;
	UINT mDsvDescriptorSize;
	UINT mCbvUavDescriptorSize;
	UINT m4xMsaaQuality;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQuene;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAllocator;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;

	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	int mAppScreenWidth;
	int mAppScreenHeight;
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif 
