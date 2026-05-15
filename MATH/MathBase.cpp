#include "stdafx.h"
#include "MathBase.h"

namespace XiahGameEngine
{
	float g_FastCossinTable[ __MAX_CIRCLE_ANGLE];
	void InitializeMathLut()
	{
		// Debug버전은 Lut를 쓰는게 더 느리다
		// 그러니 Release버전에서 굉장한 속도 차이를 낸다. 그래서 이걸 쓴다. 5년만에

		int i;

 		for (i = 0 ; i < __MAX_CIRCLE_ANGLE ; i++)
		{
			g_FastCossinTable[i] = (float)sin((double)i * _PI / __HALF_MAX_CIRCLE_ANGLE);
		}
	}
};