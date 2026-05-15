/****************************************************************************************************
	파 일 명:   CUIToolTip.h
	만든날자:	2004/02/27  13:53
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUITOOLTIP_40298151007C_INCLUDED
#define _INC_CUITOOLTIP_40298151007C_INCLUDED

#include "CUIStaticText.h"

using namespace std;

namespace XiahGameEngine
{

#define MAXIMUM_LINE_COUNT_2 30
#define VERTICAL_DISTANCE_2 20


	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-27
	 */
	class CUIToolTip 
		: public CUIStaticText
	{
	public:
		CUIToolTip(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIToolTip();

		virtual void Create(sCtrlData& data);

		XIAHGE_API void Draw();
		XIAHGE_API void SetToolTip( BYTE byType, sRect* rtRect, BYTE byCount, ...);
		XIAHGE_API void AddToolTip( LPCTSTR szTip, D3DCOLOR color = D3DCOLOR_XRGB( 255, 255, 255));
		XIAHGE_API void AddToolTip( LPCTSTR szTip, BYTE byFontSize, D3DCOLOR color = D3DCOLOR_XRGB( 255, 255, 255));

		XIAHGE_API void SetGapLine(BYTE byGapLine);

	private:

		sRect					m_rtFrameRect;
		BYTE					m_byLineCount;
		CText2D					m_Text2D[MAXIMUM_LINE_COUNT_2];
		sRect					m_rtTextRect[MAXIMUM_LINE_COUNT_2];
		LPDIRECT3DVERTEXBUFFER9	m_pVB;
		VT_TLVertex				m_Vertex[4];

		BYTE			m_byGapLine;

		void SetVB( D3DCOLOR FrameColor);
	};
};

#endif /* _INC_CUITOOLTIP_40298151007C_INCLUDED */
