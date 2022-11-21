#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_

#include <DirectXMath.h>
#include <Windows.h>

class MathHelper
{
public:
	MathHelper();
	~MathHelper();

	static DirectX::XMFLOAT4X4 Identity4x4();
};

#endif
