/****************************************************************************************************
	파 일 명:   CUIComboBox.h
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUICOMBOBOX_401D2BFA0196_INCLUDED
#define _INC_CUICOMBOBOX_401D2BFA0196_INCLUDED

#include "CUIButton.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 * 콤보 박스 컨트롤 인터페이스
	 * \date 2004-02-27
	 */
	class CUIComboBox 
		: public CUIButton
	{
	public:
		CUIComboBox(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIComboBox();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

	};
};

#endif /* _INC_CUICOMBOBOX_401D2BFA0196_INCLUDED */
