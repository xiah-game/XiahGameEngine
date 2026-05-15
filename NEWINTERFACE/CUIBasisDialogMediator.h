/****************************************************************************************************
	파 일 명:   CUIBasisDialogMediator.h
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIBASISDIALOGMEDIATOR_401D2ACA037A_INCLUDED
#define _INC_CUIBASISDIALOGMEDIATOR_401D2ACA037A_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	struct sChangeData;
	struct sGetData;
	class CUIControl;

	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-27
	 */
	class CUIBasisDialogMediator 
	{
	protected:
		CUIBasisDialogMediator();

	public:
		virtual ~CUIBasisDialogMediator();

		virtual void ControlChanged(CUIControl* pControl, int nID = -1) = 0;

		virtual void ReCreate() = 0;
		virtual void Destroy(void) = 0;

		virtual void Draw() = 0;
		virtual void Show(const int nSubID = -1) = 0;
		virtual void Hide(const int nSubID = -1) = 0;

		virtual void UpDate(sMouseEvent& mouse, int nBefore = 0) = 0;
		virtual void DataChange(sChangeData& data) = 0;
		virtual void GetData(sGetData& data) = 0;
	};
};

#endif /* _INC_CUIBASISDIALOGMEDIATOR_401D2ACA037A_INCLUDED */
