#include "MathHelper.h"

MathHelper::MathHelper()
{
}

MathHelper::~MathHelper()
{
}


DirectX::XMFLOAT4X4 MathHelper::Identity4x4() 
{
	return DirectX::XMFLOAT4X4 (
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

