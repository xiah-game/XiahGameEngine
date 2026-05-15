#pragma once

// EffectRes.cpp & HCHFile.cpp 가 동시에 include한다. 

#include <d3dx9.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "waterfall.h"
#include "thunder.h"


namespace XiahGameEngine
{
#pragma warning( disable: 4786)


// define
#define		D3DRGBA(r, g, b, a)		((((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define		RGBA_MAKE(r, g, b, a)	((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))


// type define
typedef		std::basic_string<TCHAR>		MyString;
typedef		D3DXVECTOR3		VECTOR;
typedef		D3DXMATRIX		MATRIX;

}; // namesapce