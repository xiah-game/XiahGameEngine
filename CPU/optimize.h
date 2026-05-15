#pragma once

namespace XiahGameEngine
{
	extern BOOL	s_3DNow;
	extern BOOL	s_SSE;
	extern BOOL	s_SSE2;

	extern float (*nDot)(float *a,float *b);
	extern float (*nDot4)(float *a,float *b);
	extern float (*nSQRT)(float x);
	extern void (__fastcall *nVectorNormalize)(float *d);

	// BASE
	float _Dot(float *a,float *b);
	float _Dot4(float *a,float *b);
	float _SQRT(float x);
	void __fastcall _VectorNormalize(float *d);


	// SSE
	float SSE_SQRT(float x);
	void __fastcall SSE_VectorNormalize(float *d);
	float SSE_Dot(float *a,float *c);
	float SSE_Dot4(float *a,float *c);


	// 3D_NOW
	float AMD_3DNow_SQRT(float x);
	void __fastcall AMD_3DNow_VectorNormalize(float *d);
	  
	void Assign_Optimize_Func();

}