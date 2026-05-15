/****************************************************************************************************
	파 일 명:   CUIStaticText.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIStaticText.h"

using namespace std;

namespace XiahGameEngine
{
	CUIStaticText::CUIStaticText(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID),
																	m_bDummyType(false), m_nTextureType(0), m_pOutTexture(NULL)
	{
	}

	CUIStaticText::~CUIStaticText()
	{
	}

	/**
	 * Create
	 * \param data 컨트롤 정보
	 */
	void CUIStaticText::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
		
		if(data.nType == STATICDUMMY)
			m_bDummyType = true;
	}

	void CUIStaticText::ReCreate()
	{
		CUIControl::ReCreate();
	}

	void CUIStaticText::Destroy()
	{
		CUIControl::Destroy();

		if(m_pOutTexture)
			m_pOutTexture->Release(), m_pOutTexture = NULL;
	}

	void CUIStaticText::MouseCheck(sMouseEvent& mouse)
	{
		CUIControl::MouseCheck(mouse);

		if(m_bDummyType)
		{
			if(mouse.nEventType == 1) // IN
			{
				if(mouse.bLButton)
				{
					LPARAM lParam;

					// 현재 시아 구조상 들어가게 되었음.
//					if(g_nTempPostMessage)
//					{
//						lParam = MAKELPARAM(m_nID, g_nTempPostMessage);
//						g_nTempPostMessage = 0;
//					}
//					else
					{
						lParam = m_nID;
					}

					PostMessage(g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, (WPARAM)m_nParentID, (LPARAM)lParam);

					mouse.nEventType = 4;
				}
			}
		}

	}

	/**
	 * Draw
	 */
	void CUIStaticText::Draw()
	{
		if(!m_bShow)
			return;

		if(!m_bDummyType)
		{
			if(m_nTextureType == 1)
			{
				g_pDirect3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
				g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
				g_pDirect3DDevice->SetRenderState(D3DRS_ALPHAREF, 0 );

				g_Device.SetTexture(0, m_pOutTexture);
				//g_pDirect3DDevice->SetTexture(0, m_pOutTexture );
				g_Device.SetStreamSource( m_pVB, sizeof(VT_TLVertex));
				g_Device.SetFVF(D3DFVF_TLVERTEX);
				//g_pDirect3DDevice->SetFVF(D3DFVF_TLVERTEX);
				g_pDirect3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			}
			else
			{
				CUIControl::Draw();
			}
		}

		if(m_pText2D)
			m_pText2D->Render();
	}

	/**
	 * 정보 변경
	 * \param data 변경 내용
	 */
	void CUIStaticText::DataChange(sChangeData& data)
	{
		switch(data.nType)
		{
		case TYPE:	// 정적 타입
			{
				if(data.nValue1 == STATICDUMMY)
					m_bDummyType = true;
				else
					m_bDummyType = false;
			}
			break;
		case OUTSIDE_TEXTURE:	// 외부 텍스쳐
			{
				m_nTextureType = data.nValue1;
				m_pOutTexture = (LPDIRECT3DTEXTURE9)data.dwPtr;
				// TODO: 텍스쳐 포인터 CData 수정
			}
			break;
		default:
			CUIControl::DataChange(data);
			break;
		}
	}
};