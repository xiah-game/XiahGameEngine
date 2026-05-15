 /****************************************************************************************************
	파 일 명:   CUISpinCtrl.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUISpinCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	CUISpinCtrl::CUISpinCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIButton(pMeditatorRef, nID, nParentID)
	{
	}

	CUISpinCtrl::~CUISpinCtrl()
	{
	}

	void CUISpinCtrl::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUISpinCtrl::MouseCheck(sMouseEvent& mouse)
	{
		CUIControl::MouseCheck(mouse);
	}

	void CUISpinCtrl::Draw()
	{
	}
};

