/****************************************************************************************************
	파 일 명:   CUIButton.cpp
	만든날자:	2004/02/24  15:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIButton.h"
#include "../XiahPak.h"

using namespace std;

namespace XiahGameEngine
{

	/**
	 * 생성자
	 * \param pMeditatorRef 
	 * \param nID 
	 * \param nParentID 
	 */
	CUIButton::CUIButton(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID), m_pPointTex(NULL), m_pHighlightTex(NULL),
		m_dwEventTime(0), m_nPointResID(0), m_nHighlightResID(0), m_nStep(0), m_nCurrent(-1), m_nDownCount(0)
	{
	}

	CUIButton::~CUIButton()
	{
	}

	/**
	* Create
	* \param data 생성 정보
	*/
	void CUIButton::Create(sCtrlData& data)
	{
		m_nPointResID = data.nResID2;		// Point Res
		m_nHighlightResID = data.nResID3;	// Highlight Res

		CUIControl::Create(data);
	}

	void CUIButton::ReCreate()
	{
		CreateVB();
	}

	/**
	* 해제
	*/
	void CUIButton::Destroy(void)
	{
		CUIControl::Destroy();
		
		XiahPak::ReleaseRes( m_nPointResID);
		m_pPointTex = NULL;

		XiahPak::ReleaseRes( m_nHighlightResID);
		m_pHighlightTex = NULL;
	}

	/**
	* CreateVB
	*/
	void CUIButton::CreateVB()
	{
		CUIControl::CreateVB();

		if(m_nPointResID)
			m_pPointTex = XiahPak::GetTexture( m_nPointResID, TRUE);

		if(m_nHighlightResID)
			m_pHighlightTex = XiahPak::GetTexture( m_nHighlightResID, TRUE);
	}

	/**
	* 마우스 검사
	* \param mouse 마우스 이벤트 정보
	*/
	void CUIButton::MouseCheck(sMouseEvent& mouse)
	{
		if(m_nCurrent != -1)
			return;

		CUIControl::MouseCheck(mouse);

		if(mouse.nEventType == 1) // IN
		{
			if(m_nDownCount <= 0)
			{
				if(mouse.bLButton)
				{				
					m_nStep = HIGHLIGHT;
					mouse.nEventType = 2; //
				}
				else
				{
					if(m_nStep == HIGHLIGHT)
					{
						// [3/16/2004] 앞으로 고쳐야 할 형식
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

						mouse.nEventType = 4;
						m_nStep = POINTED;
					}
					else
					{
						m_nStep = POINTED;
					}
				}
			}
			else
			{
				if(XiahInput::g_bLButtonDown)
					--m_nDownCount;

				m_nStep = NORMAL;
			}

			Changed();			
		} // if(mouse.nEventType == 1) // IN
		else
		{
			m_nStep = NORMAL;
		}
	}


	/**
	* Draw
	*/
	void CUIButton::Draw()
	{
		if(!m_bShow)
			return;

		int nStep = m_nStep;

		// 
		if(m_nCurrent != -1)
			nStep = m_nCurrent;

		if(m_nAlpha != 0)
		{
			g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		}

		switch(nStep)
		{
		case NORMAL:
			g_Device.SetTexture(0, m_pTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pTexture);
			break;
		case POINTED:
			g_Device.SetTexture(0, m_pPointTex);
			//g_pDirect3DDevice->SetTexture( 0, m_pPointTex);
			break;
		case HIGHLIGHT:
			g_Device.SetTexture(0, m_pHighlightTex);
			//g_pDirect3DDevice->SetTexture( 0, m_pHighlightTex);
			break;
		default:
			g_Device.SetTexture(0, m_pTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pTexture);
			break;
		}

		g_Device.SetStreamSource( m_pVB, sizeof(VT_TLVertex));
		g_Device.SetFVF(D3DFVF_TLVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX);
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

		// 문자열 출력
		if(m_pText2D)
			m_pText2D->Render();
	}

	/**
	 * 정보 변경
	 * \param &data 변경 내용
	 */
	void CUIButton::DataChange(sChangeData &data)
	{
		switch(data.nType)
		{
		case TYPE:
			m_nDownCount = data.nValue1;
			break;
		case CURRENT_INDEX: // Current
			m_nCurrent = data.nValue1;
			break;
		default:
			CUIControl::DataChange(data);
			break;
		}
	}

	/**
	 * Get Data
	 * \param &data 가져올 정보
	 */
	void CUIButton::GetData(sGetData &data)
	{
		switch(data.nType)
		{
		case GET_CURRENT_INDEX: // Current Index
			data.nValue1 = m_nCurrent;
			break;
		default:
			CUIControl::GetData(data);
			break;
		}
	}
};
