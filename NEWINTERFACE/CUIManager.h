/****************************************************************************************************
	파 일 명:   CUIManager.h
	만든날자:	2004/02/26  19:16
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI v0.1
// UI 재사용시 이벤트 방식을 Observer 패턴을 사용하면 좋을것 같음
// 툴 새로 만들어서 쓸것!!!

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIMANAGER_402015DC02CA_INCLUDED
#define _INC_CUIMANAGER_402015DC02CA_INCLUDED


#include "CUIBasisDialogMediator.h"
#include "CUIDialogClient.h"
#include "CUIFileLoad.h"


using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////////////////
namespace XiahGameEngine
{
	CUIEditCtrl *g_pActiveEditCtrl = NULL;
	CUIToolTip*	g_pToolTip_2 = NULL;
	bool g_bTip_2 = false;

	#define NOTICETYPE_OK				56
	#define NOTICETYPE_CANCEL			55

	//////////////////////////////////////////////////////////////////////////
	// 이런식으로 외부로 함수 많이 하지 말자.
	// 정리를 해야지
	// 
	//////////////////////////////////////////////////////////////////////////	
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-26
	 */
	class CUIManager 
	{
	public:
		XIAHGE_API CUIManager();
		XIAHGE_API ~CUIManager();

		XIAHGE_API bool FileLoad(const TCHAR* strFmrName, const TCHAR* strCmrName);

		XIAHGE_API void UpDate();
		XIAHGE_API void Show(const int nDlgID, const int nSubID = -1);	// 출력
		XIAHGE_API void ForwardShow(const int nDlgID);					// 앞쪽에 출력
		XIAHGE_API void Hide(const int nDlgID, const int nSubID = -1);	// 감추기
		XIAHGE_API void Draw();
		XIAHGE_API void SpecialDraw();

		XIAHGE_API void SetPosition(const int nDlgID, const int nPosX, const int nPosY); // Frame Position

		XIAHGE_API void SetFocus(const int nDlgID, const int nEditID = -1);
		XIAHGE_API void SetReleaseFocus(const int nDlgID, const int nEditID = -1);

		XIAHGE_API void SetPostMsg(const int nPostMsg)
		{
			g_nTempPostMessage = nPostMsg;
		}

		// DataChange /////////////////////////////////////////////////////////////////////////////////////////////////////
		XIAHGE_API void SetString(const int nDlgID, const int nSubID, const LPCTSTR strText, sFont* pFont = DEFAULT_FONT);
		XIAHGE_API void SetString(const int nDlgID, const int nSubID, const LPCTSTR strText, const int nType);
		XIAHGE_API void SetString(const int nDlgID, const int nSubID, const int nNumeral, const int nType = 0);	

		// 통합 설정
		// TODO: 다른 셋팅은 지우자
		XIAHGE_API void SetData(const int nDlgID, const int nSubID, const int nType, const int nValue1, const int nValue2 = 0, DWORD dwPtr = 0);

		// Get numeral type
		XIAHGE_API const int GetData(const int nDlgID, const int nSubID, const int nType);
		
		// Get String
		XIAHGE_API sString GetString(const int nDlgID, const int nSubID, const int nType = GET_STRING);

		XIAHGE_API void GetString(const int nDlgID, const int nSubID, LPTSTR strData, const int nType = GET_STRING);

		// Get Region
		XIAHGE_API void GetRegionData(const int nDlgID, const int nSubID, RECT &rtData);

		XIAHGE_API void CloseAll();
		XIAHGE_API void Destroy(const int nDlgID = -1);

		XIAHGE_API void ReCreate(const int nDlgID);

		XIAHGE_API int ProcessIME(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);



		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// 임시 함수들 왜 이렇게 많은 함수가 외부로 되는가...
		// 위도 맘같아서는 타입으로 다...
		XIAHGE_API const bool IsShow(int nDlgID, int nSubID = -1);
		XIAHGE_API const bool IsMouseOn(int nDlgID, int nSubID);
		XIAHGE_API const bool IsMouseOnFrame(int nDlgID = -1);
		XIAHGE_API const bool IsOnEditing() const;
		XIAHGE_API const bool IsFocus(int nDlgID, int nSubID = -1);

		XIAHGE_API inline const bool IsNotice();
		XIAHGE_API inline const bool IsPopMenu();
		XIAHGE_API inline const bool IsPopSubMenu();

		XIAHGE_API const bool ShowNotice(TCHAR* szText, BYTE byType = NOTICETYPE_OK, BYTE bySubType = 0, const int nPosX = 400, const int nPosY = 300);
		XIAHGE_API void HideNotice(BYTE byType);

		XIAHGE_API void MakePopMenu(int nXPos, int nYPos, int nFrameID, BYTE byButtonCount, ...);
		XIAHGE_API void MakePopSubMenu(int nDir, int nXPos, int nYPos, int nFrameID, BYTE byButtonCount, ...);
		XIAHGE_API void MakePopComboMenu(int nLenth, int nXPos, int nYPos, int nFrameID, BYTE byButtonCount, ...);
		XIAHGE_API void DeletePopMenu();
		XIAHGE_API void DeletePopSubMenu();

		XIAHGE_API void SetToolTip(const int nDlgID, const int nSubID, BYTE byToolTipType, LPCTSTR sToolTipText, BYTE byToolTipFontType = 0);
		XIAHGE_API void AddToolTip(const int nDlgID, const int nSubID, LPCTSTR sToolTipText, BYTE byFontType = 0);
		XIAHGE_API void AddToolTip(const int nDlgID, const int nSubID, BYTE byIndex, LPCTSTR sToolTipText, BYTE byFontType = 0);

		//HT_TEST (한글 테스트)
		XIAHGE_API void SetHanGul();


	private:

		std::map<int, CUIDialogClient*> m_mDlgList;			// 전체 리스트
		std::map<int, CUIDialogClient*> m_mActiveList;		// 활성화된 다이얼로그
		std::map<int, CUIDialogClient*> m_mSpecialList;		// 특별한 다이얼로그 ( 위쪽 출력 리스트 )

		int m_nActiveCount;									// 활성화중인 프레임 수
		int m_nFrameCount;									// 프레임 총 수

		int m_nBeforeEventDlg;								// 마지막으로 발생한 이벤트 창 ID
		int m_nFocusDlgID;									// 활성화된 다이얼로그
		int m_nFocusEditID;									// 활성화된 에디트
		
		int m_nCallDlgID;									// 호출된 다이얼로그 ( 프레임 )
		CUIDialogClient* m_pCallDlg;						// 호출된 다이얼로그 ( 프레임 )

	private:
		void CreateDlg(sFrameData &pFrameData, bool bSpecial = false);				// 프레임 생성
		void AddControl(sCtrlData &pCtrlData);				// 컨트롤 추가

		inline void DlgCall(const int nDlgID);				// 다이얼로그(프레임) 호출 -> m_pCallDlg에 연결

		// 기본적인 데이터 갱신
		inline void DefaultDataChange(const int nDlgID, const int nSubID, const int nChangeType, const int nValue1, const int nValue2 = 0, DWORD dwPtr = 0);

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/// TEMP
	private:
		int m_nPopMenuID;
		int m_nPopSubMenuID;
		bool m_bNotice;

		void SetPopMenuPos( RECT* rtRect, int nXPos, int nYPos, int nAngle);
		void SetPopSubMenuPos( RECT* rtRect, int nXPos, int nYPos, int nSeq);
		void SetPopSubComboPos( RECT* rtRect, int nXPos, int nYPos, int nLengh, int nSeq);
	};
};

#endif /* _INC_CUIMANAGER_402015DC02CA_INCLUDED */
