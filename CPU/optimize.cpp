#include <windows.h>
#include <math.h>
#include "optimize.h"
#include "amd3dx.h"

#pragma warning(disable:4730)	// "mixing _m64 and floating point expressions may result in incorrect code"

namespace XiahGameEngine
{
	BOOL	s_3DNow = FALSE;
	BOOL	s_SSE = FALSE;
	BOOL	s_SSE2 = FALSE;

	// BASE CODE
	void __fastcall _VectorNormalize(float *d)
	{
		float fLength = (float)sqrtf( d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
		d[0] /= fLength;
		d[1] /= fLength;
		d[2] /= fLength;
	}
	
	float _SQRT(float x)
	{
		return sqrtf(x);
	}

	float _Dot(float *a,float *b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	}

	float _Dot4(float *a,float *b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
	}


	// 아무런 Optimize 안할시
	float (*nDot)(float *a,float *b) = _Dot;
	float (*nDot4)(float *a,float *b) = _Dot4;
	float (*nSQRT)(float x) = _SQRT;
	void (__fastcall *nVectorNormalize)(float *d) = _VectorNormalize;

	////////////////////////////////////////////////////////////////////////

	// INLINE 이면 이루틴을 사용한다
	_declspec(naked)
	float SSE_Dot4(float *a,float *c)
	{
		_asm {
												; (stack status)
				mov	eax, DWORD PTR [esp+4]		; *a 
				mov ecx, DWORD PTR [esp+8]		; *c

				fld DWORD PTR [eax]				; a0
				fmul DWORD PTR [ecx]			; a0*c0

				fld DWORD PTR [eax+4]			; a1	| a0*c0
				fmul DWORD PTR [ecx+4]			; a1*c1	| a0*c0

				fld DWORD PTR [eax+8]			; a2	| a1*c1 | a0*c0
				fmul DWORD PTR [ecx+8]			; a2*c2	| a1*c1 | a0*c0

				fld DWORD PTR [eax+12]			; a3	|a2*c2	| a1*c1 | a0*c0
				fmul DWORD PTR [ecx+12]			; a3*c3	|a2*c2	| a1*c1 | a0*c0

				fxch st(3)						; a0*c0 | a1*c1 | a2*c2	| a3*c3
				faddp st(1), st					; a1*c1+a0*c0	| a2*c2 | a3*c3
				faddp st(1), st					; a2*c2+a0*c0+a1*c1	| a3*c3
				faddp st(1), st					; a3*c3+a2*c2+a0*c0+a1*c1
				ret
		}
	}

	// INLINE 이면 이루틴을 사용한다
	_declspec(naked) 
	float SSE_Dot(float *a,float *c)
	{
		_asm {
				mov	eax, DWORD PTR [esp+4]		; *a
				mov ecx, DWORD PTR [esp+8]		; *c
				fld DWORD PTR [eax]				; a0
				fmul DWORD PTR [ecx]			; a0*c0
				fld DWORD PTR [eax+4]			; a1	| a0*c0
				fmul DWORD PTR [ecx+4]			; a1*c1	| a0*c0
				fld DWORD PTR [eax+8]			; a2	| a1*c1 | a0*c0
				fmul DWORD PTR [ecx+8]			; a2*c2	| a1*c1 | a0*c0
				fxch st(2)						; a0*c0 | a1*c1 | a2*c2
				faddp st(1), st					; a1*c1+a0*c0	| a2*c2
				faddp st(1), st					; a2*c2+a0*c0+a1*c1
				ret
		}
	}

	// SSE SQRT
	float SSE_SQRT(float x)
	{
		float	root = 0.f;
		_asm
		{
			sqrtss		xmm0, x
			movss		root, xmm0
		}
		return root;
	}


	// 3D_NOW SQRT
	float AMD_3DNow_SQRT(float x)
	{
		float	root = 0.f;
		_asm
		{
			femms
			movd		mm0, x
			PFRSQRT		(mm1,mm0)
			punpckldq	mm0, mm0
			PFMUL		(mm0, mm1)
			movd		root, mm0
			femms
		}

		return root;
	}

	////////////////////////////////////////////////////////////////////////

	void __fastcall SSE_VectorNormalize(float *d)
	{
		__declspec(align(16)) float result[4];

		float *v = d;
		float *r = &result[0];
		float	radius = 0.f;

		if ( v[0] || v[1] || v[2] )
		{
			_asm
			{
				mov			eax, v
				mov			edx, r
				movups		xmm4, [eax]			// r4 = vx, vy, vz, X
				movaps		xmm1, xmm4			// r1 = r4
				mulps		xmm1, xmm4			// r1 = vx * vx, vy * vy, vz * vz, X
				movhlps		xmm3, xmm1			// r3 = vz * vz, X, X, X
				movaps		xmm2, xmm1			// r2 = r1
				shufps		xmm2, xmm2, 1		// r2 = vy * vy, X, X, X
				addss		xmm1, xmm2			// r1 = (vx * vx) + (vy * vy), X, X, X
				addss		xmm1, xmm3			// r1 = (vx * vx) + (vy * vy) + (vz * vz), X, X, X
				sqrtss		xmm1, xmm1			// r1 = sqrt((vx * vx) + (vy * vy) + (vz * vz)), X, X, X
				movss		radius, xmm1		// radius = sqrt((vx * vx) + (vy * vy) + (vz * vz))
				rcpss		xmm1, xmm1			// r1 = 1/radius, X, X, X
				shufps		xmm1, xmm1, 0		// r1 = 1/radius, 1/radius, 1/radius, X
				mulps		xmm4, xmm1			// r4 = vx * 1/radius, vy * 1/radius, vz * 1/radius, X
				movaps		[edx], xmm4			// v = vx * 1/radius, vy * 1/radius, vz * 1/radius, X
			}
			d[0] = result[0];
			d[1] = result[1];
			d[2] = result[2];
		}
	}

	void __fastcall AMD_3DNow_VectorNormalize(float *d)
	{
		float *v = d;
		float	radius = 0.f;

		if ( v[0] || v[1] || v[2] )
		{
			_asm
			{
				mov			eax, v
					femms
					movq		mm0, QWORD PTR [eax]
					movd		mm1, DWORD PTR [eax+8]
					movq		mm2, mm0
						movq		mm3, mm1
						PFMUL		(mm0, mm0)
						PFMUL		(mm1, mm1)
						PFACC		(mm0, mm0)
						PFADD		(mm1, mm0)
						PFRSQRT		(mm0, mm1)
						punpckldq	mm1, mm1
						PFMUL		(mm1, mm0)
						PFMUL		(mm2, mm0)
						PFMUL		(mm3, mm0)
						movq		QWORD PTR [eax], mm2
						movd		DWORD PTR [eax+8], mm3
						movd		radius, mm1
						femms
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// OPTIMIZE 함수들 지정
	void Assign_Optimize_Func()
	{
		// 3DNow 지원
		if(s_3DNow)
		{
			nSQRT = AMD_3DNow_SQRT;
			nVectorNormalize = AMD_3DNow_VectorNormalize;
		}
		else
		if(s_SSE)
		{
			// SSE code 지원
			nSQRT = SSE_SQRT;
			nVectorNormalize = SSE_VectorNormalize;
			nDot = SSE_Dot;
			nDot4 = SSE_Dot4;
		}

		// SSE2 지원
	/*	if(s_SSE2)
		{
		}
	*/
	}
}