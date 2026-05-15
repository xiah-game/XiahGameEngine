/****************************************************************************************************
	파 일 명:   CUIComboBox.cpp
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIComboBox.h"

using namespace std;

namespace XiahGameEngine
{
	CUIComboBox::CUIComboBox(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIButton(pMeditatorRef,nID,nParentID)
	{
	}

	CUIComboBox::~CUIComboBox()
	{
	}

	void CUIComboBox::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUIComboBox::MouseCheck(sMouseEvent& mouse)
	{

		CUIControl::MouseCheck(mouse);
	}

	void CUIComboBox::Draw()
	{
	}
};

