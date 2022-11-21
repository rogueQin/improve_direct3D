#include "UploadBuffer.h"


//template<typename T>
//UploadBuffer<T>::UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstanBuffer) 
//	: mIsConstantBuffer(isConstanBuffer)
//{
//	mElementByteSize = sizeof(T);
//	if (isConstanBuffer)
//		mElementByteSize = D3DUtil::CalcConstantBufferByteSize(mElementByteSize);
//
//	ThrowIfFailed(device->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize* elementCount),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&mUploadBuffer)));
//
//	ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
//}
//
//template<typename T>
//UploadBuffer<T>::~UploadBuffer()
//{
//	if (mUploadBuffer != nullptr)
//	{
//		mUploadBuffer->Unmap(0, nullptr);
//	}
//
//	mMappedData = nullptr;
//}
//
//template<typename T>
//ID3D12Resource* UploadBuffer<T>::Resource() const
//{
//	return mUploadBuffer.Get();
//}
//
//template<typename T>
//void UploadBuffer<T>::CopyData(int elementIndex, const T& data) 
//{
//	memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
//}

