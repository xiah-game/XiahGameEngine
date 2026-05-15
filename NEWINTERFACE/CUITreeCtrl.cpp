/****************************************************************************************************
	파 일 명:   CUITreeCtrl.cpp
	만든날자:	2004/02/27  13:53
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUITreeCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	CUITreeCtrl::CUITreeCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIListBox(pMeditatorRef,nID,nParentID)
	{
	}

	CUITreeCtrl::~CUITreeCtrl()
	{
	}

	void CUITreeCtrl::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUITreeCtrl::MouseCheck(sMouseEvent& mouse)
	{

		CUIControl::MouseCheck(mouse);
	}

	void CUITreeCtrl::Draw()
	{
	}
};

