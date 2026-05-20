/****************************************************************************************************
	파 일 명:   CUIManager.cpp
	만든날자:	2004/03/04  19:20
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIManager.h"

using namespace std;

namespace XiahGameEngine
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 이전 것 임시
	//=======================
	// 마우스 커서가 위치한 프레임
	// [3/15/2004] 시아 현상태를 돌리기 위해 임시 글로벌 변수
	int					g_nCurrentFrameID = 0;		// 현재 마우스 커서가 위치한 프레임
	//======================
	// 쌓여 있는 메시지 수
	int					g_nTempPostMessage = 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	XIAHGE_API CUIManager::CUIManager() : m_nActiveCount(0), m_nFrameCount(0), m_nBeforeEventDlg(0), m_nFocusDlgID(0), m_nFocusEditID(0),
		m_pCallDlg(NULL), m_nCallDlgID(-1), m_bNotice(false)
	{
		m_mDlgList.clear();
		m_mActiveList.clear();
		m_mSpecialList.clear();

		m_nPopMenuID = m_nPopSubMenuID = 0;

		g_pToolTip_2 = new CUIToolTip(NULL, -1, -1);
	}

	XIAHGE_API CUIManager::~CUIManager()
	{		

		delete g_pToolTip_2;
	}

	XIAHGE_API void CUIManager::ReCreate(const int nDlgID)
	{
		std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.find(nDlgID);

		if(iter != m_mDlgList.end())
		{
			CUIDialogClient *pDlgClient = iter->second;

			if(pDlgClient != NULL)
			{
				pDlgClient->ReCreate();
			}
		}
	}

	/**
	 * 해제
	 * \param nDlgID 제거할 ID, 미입력시 전체
	 */
	XIAHGE_API void CUIManager::Destroy(const int nDlgID)
	{
		if(nDlgID != -1)	// 해당 프레임
		{
			std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.find(nDlgID);

			if(iter != m_mDlgList.end())
			{
				CUIDialogClient *pDlgClient = iter->second;

				if(pDlgClient != NULL)
				{
					pDlgClient->Destroy();
					
					//delete pDlgClient, pDlgClient = NULL;
				}
			}

			/*
			m_mDlgList.erase(iter);
			m_mActiveList.erase(nDlgID);
			m_mSpecialList.erase(nDlgID);

			--m_nFrameCount;
			*/
		}
		else	// 전체
		{
			CUIDialogClient *pDlgClient = NULL;
			int nCount = 0;

			for(std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.begin(); iter != m_mDlgList.end(); ++iter, ++nCount)
			{
				if(iter == m_mDlgList.end())
					continue;

				pDlgClient = iter->second;

				if(pDlgClient == NULL)
					continue;

				pDlgClient->Destroy();

				delete pDlgClient, pDlgClient = NULL;
			}

			if(nCount != m_nFrameCount)
				DBG_LogFile( _T("Destroy fail"));

			// STL map clear
			m_mDlgList.clear();
			m_mActiveList.clear();
			m_mSpecialList.clear();
		}
	}

	/**
	* 갱신
	*/
	XIAHGE_API void CUIManager::UpDate()
	{
		CUIDialogClient* pDlgClient = NULL;

		sMouseEvent MEventData;

		GetCursorPos(&MEventData.ptMousePos); // xiah 에서는 윈도우 마우스 API를 이용함

		//HT_CHEAT : WINDOWSIZE
		ScreenToClient(g_EngineInfo.m_hWnd, (LPPOINT)&MEventData.ptMousePos);


		MEventData.bLButton = XiahInput::g_bLButtonOn == TRUE ? true : false; // BOOL 사용 싫어-_-
		MEventData.bRButton = XiahInput::g_bRButtonDown == TRUE ? true : false;

		MEventData.dwTime = timeGetTime();

		if(XiahInput::g_bLButtonDown || MEventData.bRButton)
		{
			if(g_pActiveEditCtrl)
				g_pActiveEditCtrl->ReleaseFocus();
		}

		if(!g_pActiveEditCtrl)
			g_nCurrentFrameID = 0;
		else
		{
			std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.find(g_pActiveEditCtrl->GetParentID());

			if (iter != m_mDlgList.end())
			{
				pDlgClient = iter->second;
				pDlgClient->UpDate(MEventData, 2);
			}
		}


		//////////////////////////////////////////////////////////////////////////
		if(m_nBeforeEventDlg != 0 && m_nBeforeEventDlg != -1)
		{
			std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.find(m_nBeforeEventDlg);

			if(iter == m_mDlgList.end())
			{
				m_nBeforeEventDlg = 0;
				return;
			}

			pDlgClient = iter->second;

			if(pDlgClient)
			{
				//MEventData.nEventType = 1;

				pDlgClient->UpDate(MEventData, 1);

				switch(MEventData.nEventType)
				{
				case 0: // 창 밖
					{
						if(g_pActiveEditCtrl)
						{
							m_nBeforeEventDlg = g_pActiveEditCtrl->GetParentID();
						}
						else
						{
							m_nBeforeEventDlg = 0;
							g_nCurrentFrameID = 0;
						}

						g_bTip_2 = false;
					}
					break;
				case 1: // 창 안 / 클릭 업 (L마우스 버튼)
				case 4:
					{
						m_nBeforeEventDlg = MEventData.nEventFrameID;

						return;
					}
					break;
				case 2:
					g_nCurrentFrameID = MEventData.nEventFrameID;
					break;
				default:
					break;
				}
			} // if(pDlgClient)
			else
			{
			}
		} // if(m_nBeforeEventDlg)

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// Special List
		for(register std::map<int, CUIDialogClient*>::reverse_iterator iterSList = m_mSpecialList.rbegin(); iterSList != m_mSpecialList.rend(); ++iterSList)
		{
			if(iterSList == m_mSpecialList.rend())
				continue;

			pDlgClient = iterSList->second;

			if(pDlgClient)
			{
				pDlgClient->UpDate(MEventData);

				switch(MEventData.nEventType)
				{
				case 0:
					break;
				case 1:
				case 2:
					{
						if(MEventData.nEventCtrlID != -1)
							m_nBeforeEventDlg = MEventData.nEventFrameID;

						//MEventData.nEventCtrlID = -1;
						if(MEventData.nEventType == 2)
							g_nCurrentFrameID = MEventData.nEventFrameID;
						return;
					}					
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else
			{
				DBG_LogFile(_T("CUIManager UpDate fail %d"), iterSList->first);
			}
		}


		// General List
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		for(register std::map<int, CUIDialogClient*>::reverse_iterator iterAList = m_mActiveList.rbegin(); iterAList != m_mActiveList.rend(); ++iterAList)
		{
			if(iterAList == m_mActiveList.rend())
				continue;

			pDlgClient = iterAList->second;

			if(pDlgClient)
			{
				pDlgClient->UpDate(MEventData);

				switch(MEventData.nEventType)
				{
				case 0:
					{
						g_bTip_2 = false;
					}					
					break;
				case 1:
				case 2:
					{
						if(MEventData.nEventCtrlID != -1)
							m_nBeforeEventDlg = MEventData.nEventFrameID;

						//MEventData.nEventCtrlID = -1;
						if(MEventData.nEventType == 2)
							g_nCurrentFrameID = MEventData.nEventFrameID;
						return;
					}					
					break;
				case 3:
					break;
				case 4:
					break;
				default:
					break;
				}
			}
			else
			{
				DBG_LogFile(_T("CUIManager UpDate fail %d"), iterAList->first);
			}
		} // for(std::vector<CUIDialogClient*>::iterator iterAList = m_vActiveList.begin(); iterAList != m_vActiveList.end(); ++iterAList)
	}


	/**
	 * 프레임 호출
	 * \param nDlgID 프레임 ID
	 */
	inline void CUIManager::DlgCall(const int nDlgID)
	{
		if(m_nCallDlgID != nDlgID)
		{
			std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.find(nDlgID);

			//YS_0728 : BUGFIX
			if ( iter != m_mDlgList.end() )
			{
				m_pCallDlg   = iter->second;
				m_nCallDlgID = nDlgID;
			}
		}
	}

	/**
	* Set Show
	* \param nDlgID 프레임 ID
	* \param nSubID 서브 컨트롤 ID, 미입력시 전체
	*/
	XIAHGE_API void CUIManager::Show(const int nDlgID, const int nSubID)
	{
		DlgCall(nDlgID);

		if(nSubID == -1)
		{
			// 프레임 전체
			if((m_mActiveList.find(nDlgID) == m_mActiveList.end()) && m_pCallDlg != NULL)
			{				
				// 객체에 전달
				m_pCallDlg->Show();

				// 활성화 리스트에 추가
				m_mActiveList.insert(std::map<int, CUIDialogClient*>::value_type(nDlgID, m_pCallDlg));

				++m_nActiveCount;
			}
		}
		else
		{
			// 프레임에 속한 컨트롤만
			m_pCallDlg->Show(nSubID);
		}		
	}

	/**
	 * Set ForwardShow
	 * \param nDlgID 프레임 ID
	 */
	XIAHGE_API void CUIManager::ForwardShow(const int nDlgID)
	{
		if(m_mSpecialList.find(nDlgID) == m_mSpecialList.end())
		{
			DlgCall(nDlgID);

			m_pCallDlg->Show();

			// Special List Add
			m_mSpecialList.insert(std::map<int, CUIDialogClient*>::value_type(nDlgID, m_pCallDlg));

			++m_nActiveCount;
		}
	}

	/**
	* Set Hide
	* \param nDlgID 프레임 ID
	* \param nSubID 서브 컨트롤 ID, 미입력시 전체
	*/
	XIAHGE_API void CUIManager::Hide(const int nDlgID, const int nSubID)
	{
		DlgCall(nDlgID);

		if(nSubID == -1)
		{// 프레임 전체

			std::map<int, CUIDialogClient*>::iterator iter = m_mDlgList.find(nDlgID);

			if (iter != m_mDlgList.end())
			{
				CUIDialogClient *pDlgClient = iter->second;

				sMouseEvent MEventData;
				pDlgClient->UpDate(MEventData, 2);
			}

			if(m_mActiveList.find(nDlgID) != m_mActiveList.end())
			{
				// 객체에 전달
				m_pCallDlg->Hide();

				// 활성화 리스트에서 제거
				m_mActiveList.erase(nDlgID);

				--m_nActiveCount;
			}
			else
			{
				if(m_mSpecialList.find(nDlgID) != m_mSpecialList.end())
				{
					m_pCallDlg->Hide();

					m_mSpecialList.erase(nDlgID);

					--m_nActiveCount;
				}
			}
		}
		else
		{// 프레임에 속한 컨트롤만
			m_pCallDlg->Hide(nSubID);
		}	
	}

	/**
	* Draw
	*/
	XIAHGE_API void CUIManager::Draw()
	{
		g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );

		g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pDirect3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		g_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

		// 출력 루틴
		register std::map<int, CUIDialogClient*>::iterator iter = m_mActiveList.begin();
		CUIDialogClient* pDlgClient = NULL;

		for( ; iter != m_mActiveList.end(); ++iter)
		{
			pDlgClient = iter->second;

			if(pDlgClient)
			{
				pDlgClient->Draw();			
			}
			else // bad ptr
			{
				DBG_LogFile( _T("UI Draw fail - [ DlgID %d ]"), iter->first);
			}
		}

		// 툴팁
		
	}

	/**
	 * SpecialDraw
	 */
	XIAHGE_API void CUIManager::SpecialDraw()
	{
		g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );

		g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pDirect3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		g_pDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

		register std::map<int, CUIDialogClient*>::iterator iter = m_mSpecialList.begin();
		CUIDialogClient* pDlgClient = NULL;

		for(; iter != m_mSpecialList.end(); ++iter)
		{
			pDlgClient = iter->second;

			if(pDlgClient)
			{
				pDlgClient->Draw();			
			}
			else // bad ptr
			{
				DBG_LogFile( _T("UI Draw fail - [ DlgID %d ]"), iter->first);
			}
		}

		// 툴팁
		if(g_bTip_2)
			g_pToolTip_2->Draw();
	}


	/**
	 * 위치 설정
	 * \param nDlgID 프레임 ID
	 * \param nPosX X 좌표
	 * \param nPosY Y 좌표
	 */
	XIAHGE_API void CUIManager::SetPosition(const int nDlgID, const int nPosX, const int nPosY)
	{
		DefaultDataChange(nDlgID, -1, 50, nPosX, nPosY);
	}

	/**
	*
	* \param nDlgID 
	* \param nEditID 
	*/
	XIAHGE_API void CUIManager::SetFocus(const int nDlgID, const int nEditID)
	{
		if(nEditID == -1)
		{
			g_nCurrentFrameID = nDlgID;
		}
		else
		{
			DefaultDataChange(nDlgID, nEditID, 25, 1);

			m_nBeforeEventDlg = nDlgID;
		}
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nEditID 
	 */
	XIAHGE_API void CUIManager::SetReleaseFocus(const int nDlgID, const int nEditID)
	{
		if(nEditID == -1)
		{
			g_nCurrentFrameID = 0;
		}
		else
		{
			DefaultDataChange(nDlgID, nEditID, 25, 0);
			
			sMouseEvent MEventData;
			m_pCallDlg->UpDate(MEventData, 2);
		}
	}

	/**
	*
	* \param strFmrName 
	* \param strCmrName 
	* \return 
	*/
	XIAHGE_API bool CUIManager::FileLoad(const TCHAR* strFmrName, const TCHAR* strCmrName)
	{		
		CUIFileLoad load;

		if(!load.CtrlLoad(strCmrName))
		{
			load.CtrlRelease();
			return false;
		} // if(!load.CtrlLoad(strCmrName))

		int nCount = load.FrameLoad(strFmrName);

		if(nCount > 0)
		{
			for(int i=0; i < nCount; ++i)					// Frame Count
			{
				sFrameData pFrameData;
				load.FrameRead(pFrameData);					// Frame Head Data

				CreateDlg(pFrameData);						// Create

				for(register int n = 0; n < pFrameData.nCtrlCount; ++n) // Ctrl Count
				{
					sCtrlData pCtrlData;
					load.CtrlRead(pCtrlData);

					AddControl(pCtrlData);

					pCtrlData.CtrlRelease();				// 정적리스트로 인한 해제
				} // for(register int n = 0; n < pFrameData.nCtrlCount; ++n) // Ctrl Count
			} // for(int i=0; i < nCount; ++i)			// Frame Count
		}
		else
		{
			// 비정상시 제거
			load.CtrlRelease();
			load.FrameEnd();

			return false;
		}

		// 제거
		load.CtrlRelease();
		load.FrameEnd();

		return true;
	}

	/**
	* 프레임 생성
	* \param &pFrameData 
	*/
	void CUIManager::CreateDlg(sFrameData &pFrameData, bool bSpecial)
	{
		CUIDialogClient *pDlgClient = new CUIDialogClient;

		pDlgClient->Create(pFrameData);

		m_mDlgList.insert(std::map<int,CUIDialogClient*>::value_type(pFrameData.nID, pDlgClient));

		if(bSpecial)
			m_mSpecialList.insert(std::map<int,CUIDialogClient*>::value_type(pFrameData.nID, pDlgClient));
		else
			m_mActiveList.insert(std::map<int,CUIDialogClient*>::value_type(pFrameData.nID, pDlgClient));

		DlgCall(pFrameData.nID);

		++m_nFrameCount;
		++m_nActiveCount;
	}

	/**
	* 컨트롤 추가
	* \param &pCtrlData 
	*/
	void CUIManager::AddControl(sCtrlData &pCtrlData)
	{
		if(m_pCallDlg)
		{
			m_pCallDlg->AddControl(pCtrlData);
		}
		else
		{
			DBG_LogFile( _T("UI AddControl fail - [ ID %d ]"), pCtrlData.nID);
		}
	}


	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param nChangeType 
	 * \param nValue1 
	 * \param nValue2 
	 */
	inline void CUIManager::DefaultDataChange(const int nDlgID, const int nSubID, const int nChangeType, const int nValue1, const int nValue2, DWORD dwPtr)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sChangeData sDataValue;

			sDataValue.nSubID	= nSubID;
			sDataValue.nType	= nChangeType;

			sDataValue.nValue1	= nValue1;
			sDataValue.nValue2	= nValue2;
			sDataValue.dwPtr	= dwPtr;

			m_pCallDlg->DataChange(sDataValue);
		} // if(m_pCallDlg)
		else
		{
			DBG_LogFile( _T("UI Manager DefaultDataChange fail - [ pDlgClient fail - DlgID %d ]"), nDlgID);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////	
	/**
	*
	* \param nDlgID 
	* \param nSubID 
	* \param strText 
	* \param pFont 
	*/
	XIAHGE_API void CUIManager::SetString(const int nDlgID, const int nSubID, const LPCTSTR strText, sFont* pFont)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sChangeData sDataValue;

			sDataValue.nSubID	= nSubID;
			sDataValue.nType	= STRING0;

			sDataValue.strText	= strText;
			sDataValue.pFont	= pFont;

			m_pCallDlg->DataChange(sDataValue);
		} // if(m_pCallDlg)
		else
		{
			DBG_LogFile( _T("UI Manager SetString (1) fail - [ pDlgClient fail - DlgID %d ]"), nDlgID);
		}
	}

	/**
	*
	* \param nDlgID 
	* \param nSubID 
	* \param strText 
	* \param nType 
	*/
	XIAHGE_API void CUIManager::SetString(const int nDlgID, const int nSubID, const LPCTSTR strText, const int nType)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sChangeData sDataValue;

			sDataValue.nSubID	= nSubID;
			sDataValue.nType	= STRING1;

			sDataValue.strText	= strText;
			sDataValue.nValue1	= nType;

			m_pCallDlg->DataChange(sDataValue);
		} // if(m_pCallDlg)
		else
		{
			DBG_LogFile( _T("UI Manager SetString (2) fail - [ pDlgClient fail - DlgID %d ]"), nDlgID);
		}
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param nNumeral 
	 * \param nType 
	 */
	XIAHGE_API void CUIManager::SetString(const int nDlgID, const int nSubID, const int nNumeral, const int nType)
	{
		DefaultDataChange(nDlgID, nSubID, STRING2, nNumeral, nType);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param nType 
	 * \param nValue1 
	 * \param nValue2 
	 */
	XIAHGE_API void CUIManager::SetData(const int nDlgID, const int nSubID, const int nType, const int nValue1, const int nValue2, DWORD dwPtr)
	{
		DefaultDataChange(nDlgID, nSubID, nType, nValue1, nValue2, dwPtr);
	}
	//////////////////////////////////////////////////////////////////////////
	
	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param &rtData 
	 */
	XIAHGE_API void CUIManager::GetRegionData(int nDlgID, int nSubID, RECT &rtData)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sGetData data;

			data.nSubID = nSubID;
			data.nType	= GET_REGION;

			m_pCallDlg->GetData(data);

			memcpy(&rtData, &data.rtData, sizeof(RECT));
		}
		else
		{
			DBG_LogFile( _T("UI Manager GetRegionData fail - [ DlgID %d ]"), nDlgID);
		}
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param nType 
	 * \return 
	 */
	XIAHGE_API const int CUIManager::GetData(const int nDlgID, const int nSubID, const int nType)
	{
		DlgCall(nDlgID);

		sGetData data;

		if(m_pCallDlg)
		{
			data.nSubID = nSubID;
			data.nType	= nType;

			m_pCallDlg->GetData(data);			
		}
		else
		{
			DBG_LogFile( _T("UI Manager GetData fail - [ DlgID %d ]"), nDlgID);
		}

		return data.nValue1;
	}

	/**
	 * 
	 * \param nDlgID 
	 * \param nSubID 
	 * \param nType 
	 * \return 
	 */
	XIAHGE_API sString CUIManager::GetString(const int nDlgID, const int nSubID, const int nType)
	{
		DlgCall(nDlgID);

		sGetData data;

		if(m_pCallDlg)
		{
			data.nSubID = nSubID;
			data.nType	= nType;

			m_pCallDlg->GetData(data);

			data.strTemp = data.strTemp2;
		}

		return data.strTemp;
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param strData 
	 * \param nType 
	 */
	XIAHGE_API void CUIManager::GetString(const int nDlgID, const int nSubID, LPTSTR strData, const int nType)
	{
		DlgCall(nDlgID);

		sGetData data;

		if(m_pCallDlg)
		{
			data.nSubID = nSubID;
			data.nType	= nType;

			m_pCallDlg->GetData(data);

			memcpy(strData, data.strTemp2, _tcslen(data.strTemp2));
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	XIAHGE_API const bool CUIManager::IsShow(int nDlgID, int nSubID)
	{
		if(nSubID != -1)
		{
			if(m_mActiveList.find(nDlgID) != m_mActiveList.end()
				|| m_mSpecialList.find(nDlgID) != m_mSpecialList.end())
			{
				DlgCall(nDlgID);

				if(m_pCallDlg)
				{
					sGetData data;

					data.nSubID = nSubID;		// 컨트롤 ID  -1일 경우 프레임 검사
					data.nType  = GET_IS_SHOW;

					GetCursorPos(&data.ptMousePos);

					m_pCallDlg->GetData(data);




					return data.bEvent;
				} // if(m_pCallDlg)
			}			
		}
		else
		{
			if(m_mActiveList.find(nDlgID) != m_mActiveList.end())
				return true;
			else if(m_mSpecialList.find(nDlgID) != m_mSpecialList.end())
				return true;
		}

		return false;
	}

	/**
	 * 프레임이나 컨트롤위에 마우스 커서가 있는지 검사
	 * \param nDlgID 
	 * \param nSubID 
	 * \return 
	 */
	XIAHGE_API const bool CUIManager::IsMouseOn(int nDlgID, int nSubID)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sGetData data;

			data.nSubID = nSubID;		// 컨트롤 ID  -1일 경우 프레임 검사
			data.nType  = GET_IS_MOUSEON;				// IsMouseOn type			

			GetCursorPos(&data.ptMousePos);

			//HT_CHEAT : WINDOWSIZE			
			ScreenToClient( g_EngineInfo.m_hWnd, (LPPOINT)&data.ptMousePos);

			m_pCallDlg->GetData(data);

			return data.bEvent;
		} // if(m_pCallDlg)

		return false;
	}

	/**
	 *
	 * \param nDlgID 
	 * \return 
	 */
	XIAHGE_API const bool CUIManager::IsMouseOnFrame(int nDlgID)
	{
		// 다음에 수정

		if(g_nCurrentFrameID)
		{
			// 아래는 채팅 프레임, DATA_WINDOW(54)
			if( g_nCurrentFrameID == 50 || g_nCurrentFrameID == 80 || g_nCurrentFrameID == 54)
				return false;

			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	 *
	 * \return 
	 */
	XIAHGE_API const bool CUIManager::IsOnEditing() const
	{
		if(g_pActiveEditCtrl)
			return true;
		else
			return false;
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \return 
	 */
	XIAHGE_API const bool CUIManager::IsFocus(int nDlgID, int nSubID)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sGetData data;

			data.nSubID = nSubID;		// 컨트롤 ID  -1일 경우 프레임 검사
			data.nType	= GET_IS_FOCUS;				// IsFocus type

			GetCursorPos(&data.ptMousePos);

			//HT_CHEAT : WINDOWSIZE			
			ScreenToClient( g_EngineInfo.m_hWnd, (LPPOINT)&data.ptMousePos);

			m_pCallDlg->GetData(data);

			return data.bEvent;
		} // if(m_pCallDlg)

		return false;
	}

	/**
	 *
	 * \param hWnd 
	 * \param msg 
	 * \param wparam 
	 * \param lparam 
	 * \return 
	 */
	XIAHGE_API int CUIManager::ProcessIME(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if(g_pActiveEditCtrl)
		{
			switch(msg)
			{
			case WM_IME_COMPOSITION:	// 글씨조합중
				g_pActiveEditCtrl->ProcessComposition(hWnd, wparam, lparam);
				break;

			case WM_CHAR:			    // 문자 넘어오기
				if(g_pActiveEditCtrl->ProcessChar( hWnd, wparam, lparam))
					break;
				else
					return 0;

			case WM_IME_NOTIFY:			// 한자입력
				g_pActiveEditCtrl->ProcessNotify( hWnd, wparam, lparam, 0);
				break;

			case WM_KEYDOWN:			// 키다운
				g_pActiveEditCtrl->ProcessKeyDown( hWnd, wparam, lparam);
				break;
			}

			//m_nBeforeEventDlg = g_pActiveEditCtrl->GetParentID();
			g_pActiveEditCtrl->RefreshText();
			return 1;
		}

		return 1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP ///
	/**
	 * 공지 여부
	 * \return 상태
	 */
	XIAHGE_API inline const bool CUIManager::IsNotice()
	{
		return m_bNotice;
	}

	/**
	 *
	 * \return 
	 */
	XIAHGE_API inline const bool CUIManager::IsPopMenu()
	{
		if(m_nPopMenuID)
			return true;
		else
            return false;
	}

	/**
	 *
	 * \return 
	 */
	XIAHGE_API inline const bool CUIManager::IsPopSubMenu()
	{
		if(m_nPopSubMenuID)
			return true;
		else
			return false;
	}
	
	/**
	 *
	 * \param szText 
	 * \param byType 
	 * \param bySubType 
	 * \param nPosX 
	 * \param nPosY 
	 * \return 
	 */
	XIAHGE_API const bool CUIManager::ShowNotice(TCHAR* szText, BYTE byType, BYTE bySubType, const int nPosX, const int nPosY)
	{
		if(IsShow(byType))
			return false;

		switch( byType)
		{
		case NOTICETYPE_OK:
			{
				SetString(byType, 1, _T("확인"));
				SetString(byType, 2, szText);
			}			
			break;
		case NOTICETYPE_CANCEL:
			{
				SetString(byType, 1, _T("확인"));
				SetString(byType, 2, _T("취소"));
				SetString(byType, 3, szText);
			}			
			break;
		case 60:
			{
				byType = NOTICETYPE_CANCEL;
				SetString(byType, 1, _T("사부 사제"));
				SetString(byType, 2, _T("연인"));
				SetString(byType, 3, szText);
			}
			break;
		case 61:
			{
				byType = NOTICETYPE_CANCEL;
				SetString(byType, 1, _T("관계 끊기"));
				SetString(byType, 2, _T("취소"));
				SetString(byType, 3, szText);
			}
			break;
		}

		SetPosition(byType, nPosX, nPosY);

		m_bNotice = true;
		g_nTempPostMessage = bySubType;

		ForwardShow(byType);

		return true;
	}

	/**
	 *
	 * \param byType 
	 */
	XIAHGE_API void CUIManager::HideNotice(BYTE byType)
	{
		Hide(byType);
		m_bNotice = false;

		g_nTempPostMessage = 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *
	 * \param nXPos 
	 * \param nYPos 
	 * \param nFrameID 
	 * \param byButtonCount 
	 * \param ... 
	 */
	XIAHGE_API void CUIManager::MakePopMenu(int nXPos, int nYPos, int nFrameID, BYTE byButtonCount, ...)
	{
		DeletePopMenu();

		m_nPopMenuID = nFrameID;

		///
		sFrameData pFrameData;

		pFrameData.nID		= nFrameID;
		pFrameData.nAlpha	= 1;

		pFrameData.nWidth	= (40*2) + (20*2);
		pFrameData.nHeight	= (40*2) + (20*2);
		
		CreateDlg(pFrameData, true);

		SetPosition(nFrameID, -60 , -60);
		//SetPosition(nFrameID, -10 , -40);

		va_list ap;
		va_start(ap, byButtonCount);

		// PopMenu Button Control 속성 설정
		for(register int i=0; i < byButtonCount; ++i)
		{
			BOOL bActive = va_arg( ap, BOOL);

			sCtrlData pCtrlData;

			if(!bActive)
			{
				pCtrlData.nType		= 6; // static
				pCtrlData.nResID	= va_arg(ap, int);
			}
			else
			{
				pCtrlData.nType		= 1; // button

				register int nResID = va_arg(ap, int);

				pCtrlData.nResID	= nResID + 1;
				pCtrlData.nResID2	= nResID + 2;
				pCtrlData.nResID3	= nResID + 3;
			}

			RECT rtRect;
			int nAngle =  i * (360 / byButtonCount);
			SetPopMenuPos(&rtRect, 0, 0, nAngle);

			pCtrlData.nID		= i;
			pCtrlData.nAlpha	= 255;

			pCtrlData.nX		= rtRect.left;
			pCtrlData.nY		= rtRect.top;
			pCtrlData.nWidth	= rtRect.right;
			pCtrlData.nHeight	= rtRect.bottom;

			pCtrlData.fU		= 1.0f;
			pCtrlData.fV		= 1.0f;
			
			AddControl(pCtrlData);

			SetToolTip(nFrameID, i, 4, va_arg( ap, TCHAR*));
			
			pCtrlData.CtrlRelease();				// 정적리스트로 인한 해제
		}

		va_end(ap);

		SetPosition(nFrameID, nXPos-60, nYPos-60);

		for(register int j=0; j < 3; ++j)
			SetData(nFrameID, j, TYPE, 2);
	}

	/**
	 *
	 * \param nDir 
	 * \param nXPos 
	 * \param nYPos 
	 * \param nFrameID 
	 * \param byButtonCount 
	 * \param ... 
	 */
	XIAHGE_API void CUIManager::MakePopSubMenu(int nDir, int nXPos, int nYPos, int nFrameID, BYTE byButtonCount, ...)
	{
		DeletePopSubMenu();

		m_nPopSubMenuID = nFrameID;

		if( nDir == 1)
			nXPos += 5;
		else
			nXPos -= 80;

		sFrameData pFrameData;

		// PopMenu Frame 속성 설정
		pFrameData.nID		= nFrameID;
		pFrameData.nAlpha	= 1;

		pFrameData.nWidth	= 90;// 72;
		pFrameData.nHeight	= 72 * byButtonCount;

		CreateDlg(pFrameData, true);

		va_list ap;
		va_start(ap, byButtonCount);

		// PopMenu Button Control 속성 설정
		for(register int i=0; i < byButtonCount; ++i)
		{
			BOOL bActive = va_arg( ap, BOOL);

			sCtrlData pCtrlData;

			if(!bActive)
			{
				pCtrlData.nType = 6;
				pCtrlData.nResID = va_arg(ap, int);
			}
			else
			{
				pCtrlData.nType = 1;

				int nResID = va_arg(ap, int);

				pCtrlData.nResID = nResID + 1;
				pCtrlData.nResID2 = nResID + 2;
				pCtrlData.nResID3 = nResID + 3;
			}

			RECT rtRect;
			SetPopSubMenuPos(&rtRect, 0, 0, i);

			pCtrlData.nID		= i;
			pCtrlData.nAlpha	= 255;

			pCtrlData.nX		= rtRect.left;
			pCtrlData.nY		= rtRect.top;
			pCtrlData.nWidth	= rtRect.right;
			pCtrlData.nHeight	= rtRect.bottom;

			pCtrlData.fU		= 1.0f;
			pCtrlData.fV		= 1.0f;

			AddControl(pCtrlData);

			SetString(nFrameID, i, va_arg( ap, TCHAR*));
			
			pCtrlData.CtrlRelease();				// 정적리스트로 인한 해제
		}

		va_end(ap);

		SetPosition(nFrameID, nXPos, nYPos);
	}

	/**
	 *
	 * \param nLenth 
	 * \param nXPos 
	 * \param nYPos 
	 * \param nFrameID 
	 * \param byButtonCount 
	 * \param ... 
	 */
	XIAHGE_API void CUIManager::MakePopComboMenu(int nLenth, int nXPos, int nYPos, int nFrameID, BYTE byButtonCount, ...)
	{
		DeletePopSubMenu();

		m_nPopSubMenuID		= nFrameID;

		sFrameData pFrameData;

		pFrameData.nID		= nFrameID;
		pFrameData.nAlpha	= 1;

		pFrameData.nWidth	= nXPos - (nXPos - nLenth);
		pFrameData.nHeight	= (nYPos + 72 * byButtonCount) - nYPos;

		CreateDlg(pFrameData, true);

		va_list ap;
		va_start( ap, byButtonCount);

		// PopMenu Button Control 속성 설정
		for(register int i=0; i < byButtonCount; ++i)
		{
			BOOL bActive = va_arg( ap, BOOL);

			sCtrlData pCtrlData;

			if( !bActive)
			{
				pCtrlData.nType = 6;
				pCtrlData.nResID = va_arg(ap, int);
			}
			else
			{
				pCtrlData.nType = 1;

				int nResID = va_arg(ap, int);

				pCtrlData.nResID = nResID + 1;
				pCtrlData.nResID2 = nResID + 2;
				pCtrlData.nResID3 = nResID + 3;
			}

			RECT rtRect;
			SetPopSubComboPos( &rtRect, 0, 0, nLenth, i);

			pCtrlData.nID = i;

			pCtrlData.nX		= rtRect.left;
			pCtrlData.nY		= rtRect.top;
			pCtrlData.nWidth	= rtRect.right;
			pCtrlData.nHeight	= rtRect.bottom;

			pCtrlData.fU = 1.0f;
			pCtrlData.fV = 1.0f;

			AddControl(pCtrlData);

			SetString(nFrameID, i, va_arg( ap, TCHAR*));
			
			pCtrlData.CtrlRelease();				// 정적리스트로 인한 해제
		}

		va_end(ap);

		SetPosition(nFrameID, nXPos - nLenth, nYPos);
	}

	/**
	 *
	 */
	XIAHGE_API void CUIManager::DeletePopMenu()
	{
		if(!m_nPopMenuID)
			return;

		DlgCall(m_nPopMenuID);

		if(m_pCallDlg)
		{
			m_pCallDlg->Destroy();

			delete m_pCallDlg, m_pCallDlg = NULL;

			m_mDlgList.erase(m_nPopMenuID);
			m_mSpecialList.erase(m_nPopMenuID);
			//m_mActiveList.erase(m_nPopMenuID);

			m_nBeforeEventDlg = 0;
			m_nPopMenuID = 0;

			--m_nFrameCount;
			--m_nActiveCount;
		}

		g_bTip_2 = false;
	}

	/**
	 *
	 */
	XIAHGE_API void CUIManager::DeletePopSubMenu()
	{
		if(!m_nPopSubMenuID)
			return;

		DlgCall(m_nPopSubMenuID);

		if(m_pCallDlg)
		{
			m_pCallDlg->Destroy();

			delete m_pCallDlg, m_pCallDlg = NULL;

			m_mDlgList.erase(m_nPopSubMenuID);
			m_mSpecialList.erase(m_nPopSubMenuID);
			//m_mActiveList.erase(m_nPopSubMenuID);

			m_nBeforeEventDlg = 0;
			m_nPopSubMenuID = 0;

			--m_nFrameCount;
			--m_nActiveCount;
		}

		g_bTip_2 = false;
	}


	/**
	 *
	 * \param rtRect 
	 * \param nXPos 
	 * \param nYPos 
	 * \param nAngle 
	 */
	void CUIManager::SetPopMenuPos( RECT* rtRect, int nXPos, int nYPos, int nAngle)
	{
		register int nTempX, nTempY;

		nTempX =  sin(nAngle * 3.14 / 180) * 40;  // RINGMENU_R
		nTempY = -cos(nAngle * 3.14 / 180) * 40;  // RINGMENU_R

		rtRect->left	= nXPos + nTempX - 26;	// RINGMENU_BUTTON_R
		rtRect->top		= nYPos + nTempY - 26;	// RINGMENU_BUTTON_R
		rtRect->right	= nXPos + nTempX + 26;	// RINGMENU_BUTTON_R
		rtRect->bottom	= nYPos + nTempY + 26;	// RINGMENU_BUTTON_R
	}

	/**
	 *
	 * \param rtRect 
	 * \param nXPos 
	 * \param nYPos 
	 * \param nSeq 
	 */
	void CUIManager::SetPopSubMenuPos(RECT* rtRect, int nXPos, int nYPos, int nSeq)
	{
		rtRect->left	= nXPos;
		rtRect->top		= nYPos			+  nSeq * 36;
		rtRect->right	= rtRect->left	+ 90; //72;
		rtRect->bottom	= rtRect->top	+ 36;
	}

	/**
	 *
	 * \param rtRect 
	 * \param nXPos 
	 * \param nYPos 
	 * \param nLengh 
	 * \param nSeq 
	 */
	void CUIManager::SetPopSubComboPos( RECT* rtRect, int nXPos, int nYPos, int nLengh, int nSeq)
	{
		rtRect->left	= nXPos;// - nLengh;
		rtRect->top		= nYPos +  nSeq * 28;
		rtRect->right	= nXPos + nLengh;
		rtRect->bottom	= rtRect->top + 28;
	}


	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param byToolTipType 
	 * \param sToolTipText 
	 * \param byToolTipFontType 
	 */
	XIAHGE_API void CUIManager::SetToolTip(const int nDlgID, const int nSubID, BYTE byToolTipType, LPCTSTR sToolTipText, BYTE byToolTipFontType)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sChangeData sDataValue;

			sDataValue.nSubID	= nSubID;
			sDataValue.nType	= SET_TOOLTIP;

			sDataValue.nValue1 = byToolTipType;
			sDataValue.strText = sToolTipText;
			sDataValue.nValue2 = byToolTipFontType;			

			m_pCallDlg->DataChange(sDataValue);
		} // if(m_pCallDlg)
		else
		{
			DBG_LogFile( _T("UI Manager SetToolTip fail - [ pDlgClient fail - DlgID %d ]"), nDlgID);
		}
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param sToolTipText 
	 * \param byFontType 
	 */
	XIAHGE_API void CUIManager::AddToolTip(const int nDlgID, const int nSubID, LPCTSTR sToolTipText, BYTE byFontType)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sChangeData sDataValue;

			sDataValue.nSubID = nSubID;
			sDataValue.nType = ADD_TOOLTIP_1;

			sDataValue.strText = sToolTipText;		
			sDataValue.nValue1 = byFontType;

			m_pCallDlg->DataChange(sDataValue);
		} // if(m_pCallDlg)
		else
		{
			DBG_LogFile( _T("UI Manager AddToolTip 1 fail - [ pDlgClient fail - DlgID %d ]"), nDlgID);
		}
	}

	/**
	 *
	 * \param nDlgID 
	 * \param nSubID 
	 * \param byIndex 
	 * \param sToolTipText 
	 * \param byFontType 
	 */
	XIAHGE_API void CUIManager::AddToolTip(const int nDlgID, const int nSubID, BYTE byIndex, LPCTSTR sToolTipText, BYTE byFontType)
	{
		DlgCall(nDlgID);

		if(m_pCallDlg)
		{
			sChangeData sDataValue;

			sDataValue.nSubID = nSubID;
			sDataValue.nType = ADD_TOOLTIP_2;

			sDataValue.nValue1 = byIndex;
			sDataValue.strText = sToolTipText;
			sDataValue.nValue2 = byFontType;			

			m_pCallDlg->DataChange(sDataValue);
		} // if(m_pCallDlg)
		else
		{
			DBG_LogFile( _T("UI Manager AddToolTip 2 fail - [ pDlgClient fail - DlgID %d ]"), nDlgID);
		}
	}


	/**
	 * 모두 닫기
	 */
	XIAHGE_API void CUIManager::CloseAll()
	{
		m_mActiveList.clear();
		m_mSpecialList.clear();

		m_nActiveCount = 0;
	}

	//HT_TEST (한글 테스트)
	XIAHGE_API void CUIManager::SetHanGul()
	{
		HIMC m_hIMC = ImmGetContext(GetForegroundWindow());
		DWORD dwConversion, dwSentence;

		ImmGetConversionStatus(m_hIMC, &dwConversion, &dwSentence);

		if(dwConversion == IME_CMODE_ALPHANUMERIC)
			ImmSetConversionStatus(m_hIMC, IME_CMODE_NATIVE, dwSentence); //한글로 전환
	}
}