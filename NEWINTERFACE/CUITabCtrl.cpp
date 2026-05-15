/****************************************************************************************************
	파 일 명:   CUITabCtrl.cpp
	만든날자:	2004/02/27  13:53
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUITabCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	CUITabCtrl::CUITabCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID)
	{
	}

	CUITabCtrl::~CUITabCtrl()
	{
	}

	void CUITabCtrl::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUITabCtrl::MouseCheck(sMouseEvent& mouse)
	{

		CUIControl::MouseCheck(mouse);
	}

	void CUITabCtrl::Draw()
	{
	}
};

