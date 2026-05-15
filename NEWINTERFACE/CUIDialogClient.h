/****************************************************************************************************
	파 일 명:   CUIDialogClient.h
	만든날자:	2004/02/27  13:27
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIDIALOGCLIENT_401D2ACB007D_INCLUDED
#define _INC_CUIDIALOGCLIENT_401D2ACB007D_INCLUDED

#include "CUIBasisDialog.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 * 인터페이스 연결 클래스
	 * \date 2004-02-27
	 */
	class CUIDialogClient 
	{
	public:
		CUIDialogClient();
		virtual ~CUIDialogClient();

		bool Create(sFrameData & pFrameData);
		bool AddControl(sCtrlData & pCtrlData);

		void ReCreate();
		void Destroy(void);

		virtual void Draw();
		virtual void Show(const int nSubID = -1);
		virtual void Hide(const int nSubID = -1);

		void UpDate(sMouseEvent& mouse, int nBefore = 0);

		// 데이터 갱신
		void DataChange(sChangeData& data);

		void GetData(sGetData& data);

	private:
		CUIBasisDialogMediator* m_pDlgMediator;

	};
};

#endif /* _INC_CUIDIALOGCLIENT_401D2ACB007D_INCLUDED */
