/****************************************************************************************************
	파 일 명:   CUIButton.h
	만든날자:	2004/02/24  15:53
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIBUTTON_401D2BF2038A_INCLUDED
#define _INC_CUIBUTTON_401D2BF2038A_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	extern int g_nTempPostMessage;

	/**
	 * \ingroup XiahGameEngine
	 * 버튼 컨트롤 인터페이스
	 * \date 2004-02-24
	 */
	class CUIButton 
		: public CUIControl
	{
	public:
		enum Type
		{
			NORMAL = 0,
			POINTED = 1,
			HIGHLIGHT = 2
		};

		CUIButton(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIButton();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

		virtual void DataChange(sChangeData& data);
		virtual void GetData(sGetData& data);

		virtual void Destroy(void);
		virtual void ReCreate();

	protected:
		
		DWORD m_dwEventTime;

		int m_nPointResID;
		int m_nHighlightResID;

		LPDIRECT3DTEXTURE9 m_pPointTex;
		LPDIRECT3DTEXTURE9 m_pHighlightTex;
		//////////////////////////////////////////////////////////////////////////
		int m_nStep;
		int m_nDownCount;
		int m_nCurrent;

		virtual void CreateVB();		

	private:

	};
};

#endif /* _INC_CUIBUTTON_401D2BF2038A_INCLUDED */
