/****************************************************************************************************
	파 일 명:   CUIControl.h
	만든날자:	2004/02/21  10:54
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

/***************************************************************************************************
XIAH

현재 인터페이스는 고정 인터페이스
***************************************************************************************************/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUICONTROL_401D2ACB005D_INCLUDED
#define _INC_CUICONTROL_401D2ACB005D_INCLUDED

#include "../text2d.h"
#include "../Interface/IExtern.h"

#include "CUIBasisDialogMediator.h"


/*
[ GetData Type ]
0 - Region
1 - IsMouseOn
2 - IsFocus
5 - String
6 - EditCtrl Send String
7 - EditCtrl Can String
10 - CurrentIndex
61 - Texture (ResID)

80 - Scroll Max
81 - Scroll Total
82 - Scroll Current
*/
enum UIGetType
{
	GET_REGION			= 0,
	GET_IS_MOUSEON		= 1,
	GET_IS_FOCUS		= 2,
	GET_IS_SHOW			= 3,

	GET_STRING			= 5,
	GET_SEND_STRING		= 6,	GET_CAN_STRING = 7,
	GET_CURRENT_INDEX	= 10,
	GET_TEXTURE			= 61,

	GET_SCROLL_MAX		= 80,
	GET_SCROLL_TOTAL	= 81,
	GET_SCROLL_CURRENT	= 82
};

enum UISetType
{
	NOEDIT			= 0,
	EDIT			= 1,
	SHOW			= 1, HIDE		= 0,
	ALL				= 0, NUMERAL	= 1,

	STRING0			= 0,
	STRING1			= 1,
	STRING2			= 2,

	VALUE1			= 10,
	VALUE2			= 11,
	TYPE			= 20,
	EDITMODE		= 26,
	EDIT_INPUT_MODE = 27,
	MAXSTRING		= 28,
	STATICLIST_SHOW = 40,
	CURRENT_INDEX	= 60,
	TEXTURE			= 61,
	OUTSIDE_TEXTURE = 62,
	COLOR			= 63,

	SCROLL_MAX		= 80,
	SCROLL_TOTAL	= 81,
	SCROLL_CURRENT	= 82,
	SCROLL_MOVE		= 83,
	SCROLL_ADD		= 84,

	SET_TOOLTIP		= 100,
	ADD_TOOLTIP_1	= 101,
	ADD_TOOLTIP_2	= 102
};



class CUIBasisDialogMediator;

using namespace std;

namespace XiahGameEngine
{
	// suck - TEMP
	extern int			g_nCurrentFrameID;
	//bool				g_bTip_2 = false;
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	#define MAX_STRING					255
	#define WM_XIAH_INTERFACE_MESSAGE	(WM_USER + 0x1234)
	
	// Control Type
	enum Type
	{
		PROGRESS = 0,
		BUTTON,
		COMBOBOX,
		EDITBOX,
		STATICLIST,// LISTBOX,
		SCROLLBAR,
		STATIC,
		TOOLTIP,
		FRAME,
		STATICDUMMY
	};


	struct sChangeData
	{
		int nSubID;				// 프레임의 하위컨트롤 ID
		int nType;				// 유형
		int nValue1, nValue2;	// 
		LPCTSTR strText;		// 문자열
		sFont* pFont;			// 폰트 정보		
		DWORD dwPtr;			// 포인터

		inline sChangeData() : nSubID(-1), nType(-1), pFont(NULL), nValue1(0), nValue2(0), dwPtr(0)
		{
		};
	};


	struct sGetData
	{
		int nSubID;					// 컨트롤 ID
		int nType;					// 유형
		int nValue1, nValue2;		// 가져올 값
		RECT rtData;				// 위치		
		bool bEvent;				// 각종 검사여부 확인 변수
		POINT ptMousePos;			// 마우스 커서 좌표
		sString strTemp;
		TCHAR strTemp2[256];

		inline sGetData() : nSubID(-1), nType(-1), nValue1(0), nValue2(0), bEvent(false)
		{
			ptMousePos.x = ptMousePos.y = 0;
			memset(strTemp2, 0, 256);
		};
	};

	/**
	 * \ingroup XiahGameEngine
	 * 컨트롤 슈퍼 클래스
	 * \date 2004-02-21
	 */
	class CUIControl 
	{
	public: // 기본 유형
		CUIControl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIControl();

		virtual void ReCreate();

		virtual void Create(sCtrlData& data);
		virtual void Changed();
		virtual void MouseCheck(sMouseEvent& mouse);

		virtual void Draw();

		void Show()
		{
			m_bShow = true;
		}

		void Hide()
		{
			m_bShow = false;
		}		

		const bool IsShow(void) const
		{
			return m_bShow;
		}

		const bool IsActive(void) const
		{
			return m_bActive;
		}

		const int GetID() const
		{
			return m_nID;
		}

		virtual const int GetParentID() const
		{
			return m_nParentID;
		}

	public:
		// 설정
		virtual void SetMovePos(const int nMoveX, const int nMoveY);

		// 정보 갱신
		virtual void DataChange(sChangeData& data);

		// GetData
		virtual void GetData(sGetData& data);

		virtual void Destroy();


	protected:
		int m_nID;			// ID
		int m_nParentID;	// 부모 ID
		int m_nType;		// 유형
		int m_nType2;		// 유형 2 (예비)
		sRect m_rtPos;		// 위치

		bool m_bShow;		// 출력 여부
		bool m_bActive;		// 활성화 여부
		int m_nResID;		// 리소스 ID
		int m_nAlpha;		// 알파값
		float m_fU, m_fV;	// U,V

		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		LPDIRECT3DTEXTURE9 m_pTexture;

		CText2D* m_pText2D;

		virtual void CreateVB(); // VB & Tex
		virtual void SetVB();

		
		// 문자열 설정
		virtual void SetString(LPCTSTR szText, sFont* pFont = DEFAULT_FONT);
		virtual void SetString(LPCTSTR szText, byte byType);
		virtual void SetString(const int nNum, const int nType);
		//virtual void SetString(const int nNum, const int nFontSize);

		void SetColor(DWORD dwColor);

		virtual void SetPos(const int nPosX, const int nPosY);
		
		virtual void SetPosDiffReturn(int &nPosX, int &nPosY);

	private:
		CUIBasisDialogMediator* m_pMediatorRef;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// TEMP
	protected:
		// ToolTip
		BOOL		m_bOnShowToolTip;
		BYTE		m_byToolTipType;	// location
		BYTE		m_byToolTipFontType;
		BYTE		m_byToolTipAddedFontType[ 20];
		BYTE		m_byAddedToolTipCount;
		sString		m_sToolTipText;
		sString		m_sToolTipTextAdded[20]; //MAX_TOOLTIP_ADDED		

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// TEMP
		void SetToolTip( BYTE byToolTipType, LPCTSTR sToolTipText, BYTE byToolTipFontType = 0);
		void AddToolTip( LPCTSTR sToolTipText, BYTE byFontType = 0);
		void AddToolTip( BYTE byIndex, LPCTSTR sToolTipText, BYTE byFontType = 0);
	};
};

#endif /* _INC_CUICONTROL_401D2ACB005D_INCLUDED */
