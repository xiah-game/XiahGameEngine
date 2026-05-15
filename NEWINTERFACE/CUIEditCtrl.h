/****************************************************************************************************
	파 일 명:   CUIEditCtrl.h
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIEDITCTRL_401D2C1101A5_INCLUDED
#define _INC_CUIEDITCTRL_401D2C1101A5_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	extern int g_nTempPostMessage;

	/**
	 * \ingroup XiahGameEngine
	 * 에디트 컨트롤 인터페이스
	 * \date 2004-02-27
	 */
	class CUIEditCtrl 
		: public CUIControl
	{
	public:
		CUIEditCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIEditCtrl();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);

		virtual void Draw();

		virtual void DataChange(sChangeData& data);
		virtual void GetData(sGetData& data);

		virtual void ProcessComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
		bool ProcessChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
		void ProcessNotify(HWND hWnd, WPARAM wParam, LPARAM lParam, int nNumber);
		void ProcessKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
		void RefreshText(sFont* pFont = DEFAULT_FONT);

		// temp
		void ReleaseFocus();
	protected:
		bool IsLastHangul();
		bool Check_Accept_DBCS();
		void ShowCarret();
		void HideCarret();

		void SetFocus();	// temp

		virtual void SetString(LPCTSTR szText, sFont* pFont = DEFAULT_FONT);
		virtual void SetString(const int nNum, const int nType);

	private:
		
		int	m_CNumber;
		int	m_CanMax;		
		int	m_carret;
		bool m_bInputMode; // false - 모든 문자 , true - 숫자
		bool m_bEditOn;
		bool m_bEditFlag;
		bool m_passmode;
		unsigned long m_fTime;
		int m_nMaxString;	// 최대 문자열

		TCHAR	m_Text[MAX_STRING];
		TCHAR	m_CanText[MAX_STRING];
		TCHAR	m_Cstr[10];

		sString	m_strSendText;
		TCHAR	m_strInputText[MAX_STRING];
	};
}

#endif /* _INC_CUIEDITCTRL_401D2C1101A5_INCLUDED */
