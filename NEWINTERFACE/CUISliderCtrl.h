/****************************************************************************************************
	파 일 명:   CUISliderCtrl.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUISLIDERCTRL_401D2C05038A_INCLUDED
#define _INC_CUISLIDERCTRL_401D2C05038A_INCLUDED

#include "CUIScrollBar.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-27
	 */
	class CUISliderCtrl 
		: public CUIScrollBar
	{
	protected:
		int m_nMax;
		int m_nCur;

	public:
		CUISliderCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUISliderCtrl();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

		const int GetCur() const;

	};
};

#endif /* _INC_CUISLIDERCTRL_401D2C05038A_INCLUDED */
