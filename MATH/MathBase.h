#pragma once

//---------------------------------------------------------------------------------------
#define _PI							3.14159265358979323846f		
#define _2_PI						6.28318530717958623200f		
#define _PI_2						1.57079632679489661923f		
#define _INV_PI						0.31830988618379069122f		
#define _DEGTORAD(x)				((x) * (_PI / 180.0f))
#define _RADTODEG(x)				((x) * (180.0f / _PI))
#define _EPSILON					1.0e-5f

/*
	Cos, Sin은 LookUpTable을 만드는게 훨씬 빠르다.

	Sqrt함수는 그냥 Runtime함수쓰는게 훨~!!!! 빠르다
*/

//---------------------------------------------------------------------------------------
#define __MAX_CIRCLE_ANGLE      512
#define __HALF_MAX_CIRCLE_ANGLE (__MAX_CIRCLE_ANGLE/2)
#define __QUARTER_MAX_CIRCLE_ANGLE (__MAX_CIRCLE_ANGLE/4)
#define __MASK_MAX_CIRCLE_ANGLE (__MAX_CIRCLE_ANGLE - 1)

namespace XiahGameEngine
{
	extern float g_FastCossinTable[ __MAX_CIRCLE_ANGLE];
	void InitializeMathLut();

	//---------------------------------------------------------------------------------------
	// FloatInt변환 함수
	inline void FloatToInt(int *int_pointer, float f) 
	{
		__asm  fld  f
		__asm  mov  edx,int_pointer
		__asm  FRNDINT
		__asm  fistp dword ptr [edx];
	}

	//---------------------------------------------------------------------------------------
	// Lut를 쓰는 Sin함수
	inline XIAHGE_API float Fast_Cos(float n)
	{
		float f = n * __HALF_MAX_CIRCLE_ANGLE / _PI;
		int i;
		FloatToInt(&i, f);
		if (i < 0)
		{
			return g_FastCossinTable[((-i) + __QUARTER_MAX_CIRCLE_ANGLE)&__MASK_MAX_CIRCLE_ANGLE];
		}
		else
		{
			return g_FastCossinTable[(i + __QUARTER_MAX_CIRCLE_ANGLE)&__MASK_MAX_CIRCLE_ANGLE];
		}
	}

	//---------------------------------------------------------------------------------------
	// Lut를 쓰는 Cos함수
	inline XIAHGE_API float Fast_Sin(float n)
	{
		float f = n * __HALF_MAX_CIRCLE_ANGLE / _PI;
		int i;
		FloatToInt(&i, f);
		if (i < 0)
		{
			return g_FastCossinTable[(-((-i)&__MASK_MAX_CIRCLE_ANGLE)) + __MASK_MAX_CIRCLE_ANGLE];
		}
		else
		{
			return g_FastCossinTable[i&__MASK_MAX_CIRCLE_ANGLE];
		}
	}
};

//---------------------------------------------------------------------------------------
// 수학함수 정의

#define SIN(a)		Fast_Sin(a)
#define COS(a)		Fast_Cos(a)
#define TAN(a)		tanf(a)
#define ASIN(a)		asinf(a)
#define ACOS(a)		acosf(a)
#define ATAN(a)		atanf(a)
#define ATAN2(a)	atan2f(a)
#define ABS(a)		fabsf(a)
#define MIN(a,b)	((a)<(b)?(a):(b))
#define MAX(a,b)	((a)>(b)?(a):(b))
#define SQRT(a)		(float)sqrt(a)


