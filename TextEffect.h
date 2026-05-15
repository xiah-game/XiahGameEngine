#pragma once
#include "Text2D.h"


namespace XiahGameEngine
{
	struct sTEXTEFFECTDATA
	{
		CText2D		Text2D;
		sRect		rcRect;
		TCHAR		szChar[50];
		D3DCOLOR	Color;
		BYTE		BackColor;
		DWORD		dwElapsedTime;
		float		fYPos;
		sFont*		pFont;
		BYTE		byType;
	};

	typedef std::list<sTEXTEFFECTDATA*> TEXTEFFECTLIST;

	// 타격시 나오는 숫자를 관리하는 클래스
	class CTextEffect
	{
		sFont*		m_pFont;		// default font
		DWORD		m_dwLifeTime;

	public:
		XIAHGE_API CTextEffect();
		XIAHGE_API virtual ~CTextEffect();

		XIAHGE_API void Release();
		XIAHGE_API void SetFont(sFont* pFont);
		XIAHGE_API void CreateTextEffect(sRect rcRect, LPCTSTR str, D3DCOLOR Color, BYTE BackColor=0, sFont* pFont=NULL, BYTE byType=0);

		XIAHGE_API void Update(DWORD dwDeltaTime);
		XIAHGE_API void Render();

	protected:
		TEXTEFFECTLIST		m_List;

	};

	//extern XIAHGE_API CTextEffect g_TextEffect;


	/////////////////////////////////////////////////////////////////////
	// 에너지 게이지
	LPDIRECT3DVERTEXBUFFER9		g_EnergyGaugeBackVB;
	LPDIRECT3DVERTEXBUFFER9		g_EnergyGaugeFrontVB;

	extern XIAHGE_API void InitEnergyGauge();
	extern XIAHGE_API void RenderEnergyGauge(int nX, int nY,int nSize, DWORD nCur, DWORD nMax, D3DCOLOR Color, D3DCOLOR BackColor, int nHeight=10);
	extern XIAHGE_API void ReleaseEnergyGauge();

};