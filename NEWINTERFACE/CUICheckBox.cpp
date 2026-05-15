/****************************************************************************************************
	파 일 명:   CUICheckBox.cpp
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUICheckBox.h"

using namespace std;

namespace XiahGameEngine
{
	CUICheckBox::CUICheckBox(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID), 
																							m_bCheck(false), m_pCheckTex(NULL)
	{
	}

	CUICheckBox::~CUICheckBox()
	{
	}

	void CUICheckBox::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUICheckBox::MouseCheck(sMouseEvent& mouse)
	{

		CUIControl::MouseCheck(mouse);
	}

	void CUICheckBox::Draw()
	{
	}
};

