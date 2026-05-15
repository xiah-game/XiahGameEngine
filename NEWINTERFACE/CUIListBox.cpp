/****************************************************************************************************
	파 일 명:   CUIListBox.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIListBox.h"

using namespace std;

namespace XiahGameEngine
{
	CUIListBox::CUIListBox(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID)
	{
	}

	CUIListBox::~CUIListBox()
	{
	}

	void CUIListBox::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUIListBox::MouseCheck(sMouseEvent& mouse)
	{
		CUIControl::MouseCheck(mouse);
	}

	void CUIListBox::Draw()
	{
	}
};

