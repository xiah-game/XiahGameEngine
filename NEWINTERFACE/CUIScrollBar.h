/****************************************************************************************************
	파 일 명:   CUIScrollBar.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUISCROLLBAR_401D2BCA02DE_INCLUDED
#define _INC_CUISCROLLBAR_401D2BCA02DE_INCLUDED

//#include "CUIButton.h"
#include "CUISpinCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	enum SCROLL_TYPE
	{
		SCROLL_HORIZONTAL_2,
		SCROLL_VERTICAL_2
	};


	/**
	 * \ingroup XiahGameEngine
	 * 스크롤 바
	 * \date 2004-02-27
	 */
	class CUIScrollBar 
		: public CUIButton
	{
	private:
		CUISpinCtrl* m_pSpin;
		CUIButton* m_pButton;

		int	m_nStep;			// 이동 수치

	public:
		CUIScrollBar(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIScrollBar();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

		// Set
		virtual void DataChange(sChangeData& data);
		// Get
		virtual void GetData(sGetData& data);

	protected:
		int m_nTotalLine;	// 모두 총 몇줄
		int	m_nCurrLine;	// 현재 몇번째 줄		

		int m_nMaxLine;		// 보이는 총 라인

		int m_nMin, m_nMax;

		POINT m_ptBeforePos;
		bool m_bPushDown;

		int m_nScrollSendID;

		virtual void SetRegion();
		virtual void SetVB();
	};
};

#endif /* _INC_CUISCROLLBAR_401D2BCA02DE_INCLUDED */
