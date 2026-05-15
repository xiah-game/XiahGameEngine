/****************************************************************************************************
	파 일 명:   CUISpinCtrl.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUISPINCTRL_401D2BDC0196_INCLUDED
#define _INC_CUISPINCTRL_401D2BDC0196_INCLUDED

#include "CUIButton.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-27
	 */
	class CUISpinCtrl 
		: public CUIButton
	{
	public:
		CUISpinCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUISpinCtrl();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

	};
};

#endif /* _INC_CUISPINCTRL_401D2BDC0196_INCLUDED */
