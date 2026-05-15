/****************************************************************************************************
	파 일 명:   CUICheckBox.h
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUICHECKBOX_401D2ACB009C_INCLUDED
#define _INC_CUICHECKBOX_401D2ACB009C_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-05-07
	 */
	class CUICheckBox 
		: public CUIControl
	{
	protected:
		bool m_bCheck;
		LPDIRECT3DTEXTURE9 m_pCheckTex;

	public:
		CUICheckBox(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUICheckBox();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

	};
}

#endif /* _INC_CUICHECKBOX_401D2ACB009C_INCLUDED */
