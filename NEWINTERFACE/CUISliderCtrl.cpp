/****************************************************************************************************
	파 일 명:   CUISliderCtrl.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUISliderCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	CUISliderCtrl::CUISliderCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIScrollBar(pMeditatorRef,nID,nParentID)
	{
	}

	CUISliderCtrl::~CUISliderCtrl()
	{
	}

	void CUISliderCtrl::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	void CUISliderCtrl::MouseCheck(sMouseEvent& mouse)
	{

		CUIControl::MouseCheck(mouse);
	}

	void CUISliderCtrl::Draw()
	{
	}

	const int CUISliderCtrl::GetCur() const
	{
		return m_nCur;
	}
};