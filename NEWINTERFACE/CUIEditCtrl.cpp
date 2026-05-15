/****************************************************************************************************
	파 일 명:   CUIEditCtrl.cpp
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIEditCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	extern CUIEditCtrl	*g_pActiveEditCtrl;

	CUIEditCtrl::CUIEditCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID),
		m_bEditOn(false), m_bEditFlag(true), m_bInputMode(false), m_nMaxString(MAX_STRING)
	{
		memset( m_strInputText, 0, MAX_STRING);
		memset( m_Text, 0, MAX_STRING);
		memset( m_Cstr, 0, 10);
		memset( m_CanText, 0, MAX_STRING);

		m_CNumber = m_CanMax = m_carret = 0;
	
		m_fTime = timeGetTime();
		m_passmode = false;
	}

	CUIEditCtrl::~CUIEditCtrl()
	{
	}

	/**
	*
	* \param data 
	*/
	void CUIEditCtrl::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}

	/**
	*
	* \param mouse 
	*/
	void CUIEditCtrl::MouseCheck(sMouseEvent& mouse)
	{
		if(mouse.dwTime - m_fTime > 300)
		{
			m_fTime = mouse.dwTime;
			m_carret = !m_carret;
		} // if(mouse.dwTime - m_fTime > 300)

		ShowCarret();

		CUIControl::MouseCheck(mouse);

		if(mouse.nEventType == 1)
		{
			if(mouse.bLButton && m_bEditFlag)
			{
				mouse.nEventType = 2;

				SetFocus();
			}

			Changed();
		}
	}

	/**
	* Render
	*/
	void CUIEditCtrl::Draw()
	{
		if(m_pText2D)
			m_pText2D->Render();
	}


	/**
	 * 문자열 설정
	 * \param szText 텍스트
	 * \param pFont 폰트
	 */
	void CUIEditCtrl::SetString(LPCTSTR szText, sFont* pFont)
	{
		if( _tcslen( szText) < 1)
		{
			memset(m_strInputText,0,sizeof(m_strInputText));
			memset(m_Text,0,sizeof(m_Text));
			memset(m_Cstr,0,sizeof(m_Cstr));

//			if(!m_pText2D)
//				m_pText2D = new CText2D;

			m_pText2D->Clear();
		}
		else
		{
			_tcscpy( m_Text, szText);

			RefreshText( pFont);
		}
	}

	/**
	 * 문자열 설정
	 * \param nNum 숫자
	 */
	void CUIEditCtrl::SetString(const int nNum, const int nType)
	{
		itoa(nNum, m_Text, 10);

//		if(!m_pText2D)
//			m_pText2D = new CText2D;

		m_pText2D->SetParentRect(&m_rtPos);
		m_pText2D->SetText(&m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT,
							(LPCTSTR)m_Text, DEFAULT_FONT, DEFAULT_COLOR);
	}

	/**
	*
	* \param hWnd 
	* \param wParam 
	* \param lParam 
	*/
	void CUIEditCtrl::ProcessComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		if(m_passmode)
			return;

		int	len = 0;
		HIMC m_hIMC = ImmGetContext(hWnd);	// ime핸들을 얻는것 ime 함수를 쓰려면 꼭 얻어야해요!

		if(lParam & GCS_RESULTSTR)		// 완성되면
		{   
			if((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)	// 현재 IME의 스트링 길이를 얻는다;.(조합중인이라 생각하심이)
			{	
				ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, m_Cstr, len);			// Cstr에 조합중인 문자열을 받아낸다. // 함수명은 똑같구 인자에 저게 붙으면 넘어온다는;;..	
				//memcpy(m_temp_Cstr,m_Cstr,sizeof(m_Cstr));
				memset( m_Cstr,0,10);													// 초기화
			}				
		}

		if(lParam & GCS_COMPSTR)		// 조합중이면
		{ 
			len = ImmGetCompositionString( m_hIMC, GCS_COMPSTR, NULL, 0);	// 조합중인 길이를 얻는다.		
			ImmGetCompositionString( m_hIMC, GCS_COMPSTR, m_Cstr, len);		// str에  조합중인 문자를 얻는다.
			m_Cstr[len] = '\0';												// 뒤에 0을 붙인다.
		}

		ImmReleaseContext( hWnd, m_hIMC);	// IME 핸들 반환!!
	}

	/**
	*
	* \param hWnd 
	* \param wParam 
	* \param lParam 
	* \return 
	*/
	bool CUIEditCtrl::ProcessChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		int	len	=0;

		if( wParam == 13)			// enter
		{
			if(_tcslen(m_Text) >= 1 && m_bEditOn == true)
			{
				// for chat edit
				if( g_nCurrentFrameID == 63)
				{
					m_strSendText = m_Text;

					memset(m_Text, 0, MAX_STRING);
					memset(m_strInputText, 0, sizeof(m_strInputText));
				}

				LPARAM lParam;

				// 현재 시아 구조상 들어가게 되었음.
				if(g_nTempPostMessage)
				{
					lParam = MAKELPARAM(m_nID, g_nTempPostMessage);
					g_nTempPostMessage = 0;
				}
				else
				{
					lParam = m_nID;
				}

				PostMessage(g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, (WPARAM)m_nParentID, (LPARAM)lParam);				
			}
			else
			{
				m_bEditOn = true;

				// 채팅 초점 해제 또는 개행
				PostMessage( g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, static_cast<WPARAM>(m_nParentID), static_cast<LPARAM>(200));

				return false;
			}
		}
		else if( wParam == 8)		// back space
		{      
			int str_len = _tcslen( m_Text);

			if(str_len > 0)
			{
				if( IsLastHangul())
				{
					m_Text[str_len - 2] = '\0';
					m_Text[str_len - 1] = '\0';
					m_Text[str_len] = '\0';
				}
				else
				{
					m_Text[str_len-1] = '\0';
				}

				ZeroMemory( m_Cstr, 10);
			}
			else
			{	// 앞 라인으로 가기 위해서
				PostMessage(g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, static_cast<WPARAM>(m_nParentID), static_cast<LPARAM>(130+m_nID));
			}
		}
		else if( wParam == 27)		// esc
		{
			return false; //ReleaseFocus();
		}
		else if( wParam == '|')		// | (or) 줄바꿈표시인데..막아주자
		{
			return false;
		}
		else if( wParam == VK_TAB)	// tab
		{
			PostMessage( g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, (WPARAM)m_nParentID, (LPARAM)100);
			return false;
		}
		else
		{  
			if(m_bInputMode)
			{
				// 숫자 입력모드				
				len = _tcslen( m_Text);

				if(len > m_nMaxString)
					return true;

				TCHAR tempchar = wParam & 0xff;

				if(tempchar >= '0' && tempchar <= '9')
				{
					m_Text[len] = tempchar;
					m_Text[len+1] = 0;
					m_bEditOn = true;
				}
				else
					return false;
			}
			else
			{
				// 문자 입력모드
				static int Check_LastDBCS = 0;

				len = _tcslen( m_Text);
				int ext = IsDBCSLeadByte(wParam & 0xff)? 1:0;

				if(ext)
				{
					// 입력문자가 DBCS 이다,  버퍼를 넘거나 받을수 없으면 리턴
					if( len+ext > m_nMaxString && Check_Accept_DBCS() == false)
					{
						PostMessage( g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, static_cast<WPARAM>(m_nParentID), static_cast<LPARAM>(m_nID)+190);
						memset(m_Cstr,0,sizeof(m_Cstr));
						return true;
					}

					m_Text[len] = wParam & 0xff;		//  넘어온 문자를 문자열에 넣기..
					m_Text[len+1] = 0;					//  뒤에 0붙이깃!!				
				}
				else
				{
					// 입력문자가 ASCII
					if(len + ext > m_nMaxString)
					{
						PostMessage( g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, static_cast<WPARAM>(m_nParentID), static_cast<LPARAM>(m_nID)+190);
						memset(m_Cstr,0,sizeof(m_Cstr));
						return true;
					}

					m_Text[len] = wParam & 0xff;		//  넘어온 문자를 문자열에 넣기..
					m_Text[len+1] = 0;					//  뒤에 0붙이깃!!
				}

				m_bEditOn = true;
			}
		}

		return true;
	}

	/**
	*
	* \param hWnd 
	* \param wParam 
	* \param lParam 
	* \param nNumber 
	*/
	void CUIEditCtrl::ProcessNotify(HWND hWnd, WPARAM wParam, LPARAM lParam, int nNumber)
	{
		DWORD dwBufLen =0;    // 버퍼길이
		LPCANDIDATELIST lpCandList = NULL;   // 특수문자 리스트
		int i = 0;

		LPCANDIDATELIST pCandList;

		HIMC m_hIMC = ImmGetContext( hWnd);

		switch(wParam) 
		{
		case IMN_OPENCANDIDATE:			// 처음 열렸을때
			{
				memset(m_CanText, 0, 100);	//  특수문자가 들어갈 변수 초기화

				if(!(dwBufLen = ImmGetCandidateList( m_hIMC, 0, lpCandList, 0)))		// 리스트의 길이 받기..
					break;

				pCandList = (LPCANDIDATELIST)new TCHAR[dwBufLen];					// 길이만큼 메모리 얻기
				lpCandList = pCandList;											// 왜 이렇게 해줘야 돼는건진 모르겠는데 어느날 소스를// 보니 이렇게 돼있었음;;.. 죄송!!

				ImmGetCandidateList( m_hIMC, 0, lpCandList, dwBufLen);				// 할당받은 데로 리스트를 받는다.// 아까도 그랬지만 인자에 따라서 같은 함수가 용도가 달라짐

				m_CanMax = lpCandList->dwCount;	// 특수문자 목록의 겟수를 넣는다!

				for(i=nNumber; i < nNumber+9; ++i)  // 한번에 9개만 보이니까 9개만받는다.
				{  								
					if( i >= lpCandList->dwCount) 
						break;					// 모두 알다시피 더크면 뽀로롱~ 나간다!

					LPTSTR lpStr = (LPTSTR)lpCandList + lpCandList->dwOffset[i];	// 문자얻기;.
					_tcscpy( m_CanText + _tcslen( m_CanText), lpStr);				// 특수문자 문자열에 넣는다;.// 근데 왜 두번이나 지나서 넣는거지? -_-;.									
				}

				delete [] pCandList;			// 만든걸 날린다!

				pCandList=NULL;			// 널을 넣는다;.(안넣어도 무방하다;.)
				m_CNumber = 0;				// 현제의 특수문자 번호를 0으로 만든다;.
			}
			break;

		case IMN_CHANGECANDIDATE:		// 딴 키를 눌러서 새로운 목록으로 바꾸기라면
			{
				memset( m_CanText,0,100);	// 또 다 날린다!

				if(!( dwBufLen = ImmGetCandidateList(m_hIMC, 0, lpCandList, 0)))
					break;					// 길이를 얻는다

				pCandList = (LPCANDIDATELIST)new TCHAR[dwBufLen];		// 또 메모리 할당한다;.

				lpCandList = pCandList;								// 역시 위와 마찬가지로 어느날 이렇게;;.

				ImmGetCandidateList(m_hIMC, 0, lpCandList, dwBufLen);	// lpCandList에 목록을 얻는다;.

				for( i=m_CNumber; i<m_CNumber+9; ++i)						// 역시 현재 번호부터 9개까지
				{  
					if( i >= lpCandList->dwCount) 
						break;											// 역시 더크면 뾰로롱 나간다~

					LPTSTR lpStr = (LPTSTR)lpCandList + lpCandList->dwOffset[i];	// 위처럼 lpStr에 문자를 넣고..
					_tcscpy( m_CanText + _tcslen( m_CanText), lpStr);		// 또 복사를 한다;. -_-;.// 난 왜 이렇게 해놓은거지? -_-;.						
				}

				delete [] pCandList;           // 역시 할당한것을 날리고
				pCandList=NULL;             // 리스트에 널을 넣는다.				
			}
			break;

		case IMN_CLOSECANDIDATE:         // 목록 리스트가 닫힐경우.
			m_CanText[0]=0;              // CanText젤 앞에 0을 넣어버린다;;.
			break;
		}

		ImmReleaseContext(hWnd, m_hIMC);
	}

	/**
	*
	* \param hWnd 
	* \param wParam 
	* \param lParam 
	*/
	void CUIEditCtrl::ProcessKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		if( m_CanText[0]==0)		// 특수문자에 글씨가 없뜨면;;..// 그러니까 창이 안열여뜨면!
			return;

		if( wParam == VK_PROCESSKEY)	// 프로세스키로 들어가는거면..// 특수문자 창이 열렸을경우 이렇게// 들어가요;;..								
		{
			if( ( lParam & 0xff0000) == 4915200 || ( lParam & 0xff0000) == 4784128 )	// 왼쪽이면;;. -_-;. 무식해서 죄송 / 페이지 업
			{									
				if(m_CNumber >0)
					m_CNumber-=9;					// 영보다 크면 9빼기

				ProcessNotify(hWnd,wParam, lParam,m_CNumber);
				return;
			}

			if( ( lParam & 0xff0000) == 5046272 || ( lParam & 0xff0000) == 5308416 )	// 오른쪽이면;;.. / 페이지 다운
			{
				if( m_CNumber+9 < m_CanMax)
					m_CNumber+=9;					// 구더하기..

				ProcessNotify( hWnd,wParam, lParam,m_CNumber);
				return;
			}

			// ESC
			if( (lParam & 0xff0000) == 65536)
			{
				m_CanText[0]=0;
				return;
			}

			// Enter
#ifdef _CHINA_
			if( (lParam & 0xff0000) == 1835008)
			{
				ProcessNotify( hWnd,IMN_OPENCANDIDATE, lParam, 0);
				return;
			}
#endif
		}
	}

	/**
	*
	* \param pFont 
	*/
	void CUIEditCtrl::RefreshText(sFont* pFont)
	{
		memset(m_strInputText,0,sizeof(m_strInputText));

		if(m_passmode)
		{
			int len = _tcslen(m_Text);

			for(int i=0; i < len ; ++i)
			{
				m_strInputText[i] = '*';
			}

			if(!len)
				m_strInputText[0] =  0x20;
		}
		else
		{
			_tcscpy(m_strInputText, m_Text); // [3/4/2004] m_szInputText

			if( m_Cstr[0] != 0 && _tcslen(m_strInputText) < m_nMaxString-1)
				_tcscpy( m_strInputText + _tcslen( m_Text), m_Cstr);

			if( _tcslen(m_strInputText) < 1)
				return;

//			if(!m_pText2D)
//				m_pText2D = new CText2D;
		}

		m_pText2D->SetParentRect(&m_rtPos);
		m_pText2D->SetText( &m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT, (LPCTSTR)m_strInputText, pFont, DEFAULT_COLOR);
	}

	/**
	*
	* \return 
	*/
	bool CUIEditCtrl::IsLastHangul()
	{
		bool bHangul = false;
		int str_len = _tcslen( m_Text);

		for(int i = 0; i< str_len; ++i)
		{
			if(IsDBCSLeadByte(m_Text[i]))
			{
				// 한글
				++i;

				bHangul = true;
			}
			else
			{
				bHangul = false;
			}
		}

		return bHangul;
	}

	/**
	*
	* \return 
	*/
	bool CUIEditCtrl::Check_Accept_DBCS()
	{
		bool ch = false;
		int len = _tcslen(m_Text);

		for(int i=0; i<len; ++i)
		{
			if(IsDBCSLeadByte(m_Text[i]))
			{
				++i;

				if(i >= len)
					ch = true;	// 하나더 받아야 한다

				if(IsDBCSLeadByte(m_Text[i]))
					ch = false; // 이미 쌍을 이루었다.
			}
		} // for(int i=0;i<len;i++)

		return ch;
	}

	/**
	*
	*/
	void CUIEditCtrl::ShowCarret()
	{
		int len;

		if(m_bActive)
		{
			len = _tcslen(m_Text);

			if(len) 
			{
				if(m_passmode)
				{
					memset(m_strInputText,0,sizeof(m_strInputText));

					for(int i=0; i<len; ++i)
					{
						m_strInputText[i] = '*';
					}
				}
				else
				{
					// 조합중인 한글이 있으면?? 조합중인 한글을 표시하여 준다.
					if(m_Cstr[0] != 0)
					{
						memset(m_strInputText,0,sizeof(m_strInputText));
						_tcscpy(m_strInputText,m_Text);

						if(len < m_nMaxString-1)
							_tcscat(m_strInputText,m_Cstr);
					}
					else
					{
						memset(m_strInputText,0,sizeof(m_strInputText));
						_tcscpy(m_strInputText,m_Text);
					}
				}

				m_carret?_tcscat(m_strInputText,_T("_")) :_tcscat(m_strInputText,_T(" "));
			}
			else
			{
				// 처음 입력대기 상태
				if(m_Cstr[0] != 0)
				{
					memset(m_strInputText,0,sizeof(m_strInputText));
					_tcscat(m_strInputText,m_Cstr);
					m_carret?_tcscat(m_strInputText,_T("_")) :_tcscat(m_strInputText,_T(" "));
				}
				else
				{
					m_carret ? m_strInputText[0] = '_':m_strInputText[0] = 0x20;
				}
			}
		}
		else
		{
			len = _tcslen( m_Text);

			if(m_passmode)
			{
				memset(m_strInputText, 0, sizeof(m_strInputText));

				for(int i=0; i<len; ++i)
				{
					m_strInputText[i] = '*';
				}
			}
			else
			{
				if(len) 
				{
					// 조합중인 한글이 있으면?? 조합중인 한글을 표시하여 준다.
					if(m_Cstr[0] != 0)
					{
						memset(m_strInputText,0,sizeof(m_strInputText));
						_tcscpy(m_strInputText,m_Text);
						_tcscat(m_strInputText,m_Cstr);
					}
					else
					{
						memset(m_strInputText,0,sizeof(m_strInputText));
						_tcscpy(m_strInputText,m_Text);
					}
				}
				else
				{
					memset(m_strInputText,0,sizeof(m_strInputText));

					if(m_Cstr[0] != 0)
						_tcscat(m_strInputText,m_Cstr);

					_tcscat(m_strInputText,_T(" "));

				}
			}
		}

//		if(!m_pText2D)
//			m_pText2D = new CText2D;

		m_pText2D->SetParentRect( &m_rtPos);
		m_pText2D->SetText(&m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT,(LPCTSTR)m_strInputText, DEFAULT_FONT, DEFAULT_COLOR);
	}

	/**
	*
	*/
	void CUIEditCtrl::HideCarret()
	{
		int str_len = _tcslen(m_strInputText);

		if(str_len > 0)
		{
			if(m_strInputText[str_len -1] != '_')
				return;

			m_strInputText[ str_len - 1] = 0x20;// '\0';

//			if(!m_pText2D)
//				m_pText2D = new CText2D;

			m_pText2D->SetParentRect( &m_rtPos);
			m_pText2D->SetText(&m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT, (LPCTSTR)m_strInputText, DEFAULT_FONT, DEFAULT_COLOR);
		}
	}

	/**
	 * 정보 변경
	 * \param data 변경 내용
	 */
	void CUIEditCtrl::DataChange(sChangeData& data)
	{
		CUIControl::DataChange(data);

		switch(data.nType)
		{
		case TYPE:		// 패스워드 모드
			m_passmode = true;
			break;

		case 25:		// 활성화 설정
			{
				if(data.nValue1 == 0)
					ReleaseFocus();
				else
					SetFocus();
			}
			break;

		case EDITMODE: // 입력 가능 설정
			{
				if(data.nValue1 == 0)
					m_bEditFlag = false;
				else
					m_bEditFlag = true;
			}
			break;

		case EDIT_INPUT_MODE: // 입력 모드
			{
				if(data.nValue1 == 0)
					m_bInputMode = false;
				else
					m_bInputMode = true;
			}
			break;

		case MAXSTRING: // Max String
			{
				if(data.nValue1)
					m_nMaxString = data.nValue1;				
			}
			break;
		}
	}

	/**
	 * Get Data
	 * \param data 가져올 정보
	 */
	void CUIEditCtrl::GetData(sGetData& data)
	{
		switch(data.nType)
		{
		case GET_STRING:		// GET_STRING
			{
				memcpy(data.strTemp2, m_Text, _tcslen(m_Text) );
			}
			break;
		case GET_SEND_STRING:	// GET_SEND_STRING
			{
				memcpy(data.strTemp2, m_strSendText, m_strSendText.size());
			}
			break;
		case GET_CAN_STRING:	// GET_CAN_STRING
			{
				memcpy(data.strTemp2, m_CanText, _tcslen(m_CanText) );
			}
			break;
		default:
			CUIControl::GetData(data);
			break;
		}
	}

	// temp
	/**
	 *
	 */
	void CUIEditCtrl::SetFocus()
	{
		if(g_pActiveEditCtrl)
			g_pActiveEditCtrl->ReleaseFocus();

		g_pActiveEditCtrl = this;

		m_bActive = m_bEditOn = true;
	}

	/**
	 *
	 */
	void CUIEditCtrl::ReleaseFocus()
	{
		g_pActiveEditCtrl = NULL;

		m_bActive = m_bEditOn = false;

		HideCarret();
	}
}

