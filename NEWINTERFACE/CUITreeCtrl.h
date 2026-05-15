/****************************************************************************************************
	파 일 명:   CUITreeCtrl.h
	만든날자:	2004/02/27  13:53
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUITREECTRL_401D2C47003E_INCLUDED
#define _INC_CUITREECTRL_401D2C47003E_INCLUDED

#include "CUIListBox.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-27
	 */
	class CUITreeCtrl 
		: public CUIListBox
	{
	public:
		CUITreeCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUITreeCtrl();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

	};
};

#endif /* _INC_CUITREECTRL_401D2C47003E_INCLUDED */
