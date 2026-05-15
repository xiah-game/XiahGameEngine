/****************************************************************************************************
	파 일 명:   CUIListBox.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUILISTBOX_401D2C3F0213_INCLUDED
#define _INC_CUILISTBOX_401D2C3F0213_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-05-07
	 */
	class CUIListBox 
		: public CUIControl
	{
	public:
		CUIListBox(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIListBox();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

	};
};

#endif /* _INC_CUILISTBOX_401D2C3F0213_INCLUDED */
