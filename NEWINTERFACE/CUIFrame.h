/****************************************************************************************************
	파 일 명:   CUIFrame.h
	만든날자:	2004/02/24  17:36
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIFRAME_401FF7670113_INCLUDED
#define _INC_CUIFRAME_401FF7670113_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	//프레임 클래스
	//사용자 이동 제어 불가능
	//차후 윈도우 형식처럼 이동을 처리시는 프레임과 
	//기본다이어그램 클래스의 컨트롤체인지를 수정해주면 된다.

	/**
	 * \ingroup XiahGameEngine
	 * 프레임
	 * \date 2004-02-24
	 */
	class CUIFrame 
		: public CUIControl
	{
	public:
		CUIFrame(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIFrame();

		/*
		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		*/

		virtual void Draw();
	};
};

#endif /* _INC_CUIFRAME_401FF7670113_INCLUDED */
