#ifndef _D3DUTIL_H_
#define _D3DUTIL_H_

#include <windows.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <comdef.h>
#include "d3dx12.h"

class D3DUtil
{
public:
	D3DUtil();
	~D3DUtil();

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

private:

};


class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

	std::wstring ToString()const;

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif


#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

#endif 






