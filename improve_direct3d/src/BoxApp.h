#ifndef _BoxApp_H
#define _BoxApp_H

#include "D3DApp.h"
#include "UploadBuffer.h"
#include "MathHelper.h"

struct VertexColorData
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Color;
};

struct VertexTextureData
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TextureUV;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 viewProj = MathHelper::Identity4x4();
};

class BoxApp : public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance, int show);
	~BoxApp();

	BoxApp(const BoxApp& rhs) = delete;
	BoxApp& operator=(const BoxApp& rhs) = delete;

	virtual bool Initlize() override;

protected:
	virtual void Update() override;
	virtual void Draw() override;
	virtual void ResetViewPort() override;

private:
	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB;

	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> mfsByteCode;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	std::unique_ptr<MeshGeometry> mBoxGeo;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;

	DirectX::XMFLOAT4X4 mWord = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.7f * DirectX::XM_PI;
	float mPhi = DirectX::XM_PIDIV4;

};

#endif 
