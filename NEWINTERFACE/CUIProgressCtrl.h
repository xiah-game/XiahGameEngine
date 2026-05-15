/****************************************************************************************************
	파 일 명:   CUIProgressCtrl.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIPROGRESSCTRL_401D2C1C001F_INCLUDED
#define _INC_CUIPROGRESSCTRL_401D2C1C001F_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 * 진행표시 컨트롤
	 * \date 2004-02-27
	 */
	class CUIProgressCtrl 
		: public CUIControl
	{
	public:
		enum PROGRESS_DIR
		{
			LEFT_TO_RIGHT = 0,
			RIGHT_TO_LEFT
		};

		CUIProgressCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIProgressCtrl();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();
		virtual void SetVB();

		virtual void DataChange(sChangeData& data);

	protected:
		int m_nMin, m_nMax;

		int	m_nCurrentRatio;
		int m_nProgressDir;
		// LPDIRECT3DTEXTURE9 m_pBackTex; // 툴 재작업시 사용될것

		//virtual void CreateVB();
	};
};

#endif /* _INC_CUIPROGRESSCTRL_401D2C1C001F_INCLUDED */
