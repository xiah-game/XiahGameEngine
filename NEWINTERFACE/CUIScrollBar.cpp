/****************************************************************************************************
	파 일 명:   CUIScrollBar.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIScrollBar.h"

using namespace std;

namespace XiahGameEngine
{
	CUIScrollBar::CUIScrollBar(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIButton(pMeditatorRef,nID,nParentID),
		m_nStep(1), m_nTotalLine(0), m_nCurrLine(0), m_nMaxLine(0), m_nMin(0), m_nMax(0), m_pSpin(NULL), m_pButton(NULL), m_bPushDown(false),
		m_nScrollSendID(0)
	{
		m_ptBeforePos.x = m_ptBeforePos.y = 0;
	}

	CUIScrollBar::~CUIScrollBar()
	{
	}

	/**
	 * Create
	 * \param data 컨트롤 정보
	 */
	void CUIScrollBar::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
		// TODO: 툴 재작업시 스크롤 바 생성 수정 해야함
	}

	/**
	 * 마우스 검사
	 * \param mouse 마우스 이벤트 정보
	 */
	void CUIScrollBar::MouseCheck(sMouseEvent& mouse)
	{
		CUIControl::MouseCheck(mouse);

		if(!mouse.bLButton)
			m_bPushDown = false;

		if(mouse.nEventType == 1 && mouse.bLButton)
			m_bPushDown = true;

		if(m_bPushDown)
		{
			int nMove = 0;
			int nStep = 0;

			switch(m_nType2)
			{
			case SCROLL_HORIZONTAL_2:
				nMove = mouse.ptMousePos.x - m_ptBeforePos.x;
				nStep = m_rtPos.right - m_rtPos.left;
				break;
			case SCROLL_VERTICAL_2:
				nMove = mouse.ptMousePos.y - m_ptBeforePos.y;
				nStep = m_rtPos.bottom - m_rtPos.top;
				break;
			}			

			if(nMove != 0)
			{
				if(nMove > 0)
				{
					int nSize = 0;

					switch(m_nType2)
					{
					case SCROLL_HORIZONTAL_2:
						nSize = mouse.ptMousePos.x - m_rtPos.right;
						m_ptBeforePos.x = m_rtPos.right;
						break;
					case SCROLL_VERTICAL_2:
						nSize = mouse.ptMousePos.y - m_rtPos.bottom;
						m_ptBeforePos.y = m_rtPos.bottom;
						break;
					}

					m_nCurrLine += (nSize/nStep);

					if(m_nCurrLine < 0)
						m_nCurrLine = 0;

					if(m_nCurrLine > m_nTotalLine)
						m_nCurrLine = m_nTotalLine;

					if( m_nTotalLine != 0)
						m_nCurrent = m_nCurrLine * 100 / m_nTotalLine;
					else
						m_nCurrent = 0;

					SetRegion();
				}
				else
				{
					int nSize = 0;

					switch(m_nType2)
					{
					case SCROLL_HORIZONTAL_2:
						nSize = m_rtPos.left - mouse.ptMousePos.x;
						m_ptBeforePos.x = m_rtPos.left;
						break;
					case SCROLL_VERTICAL_2:
						nSize = m_rtPos.top - mouse.ptMousePos.y;
						m_ptBeforePos.y = m_rtPos.top;
						break;
					}					

					m_nCurrLine -= (nSize/nStep);

					if(m_nCurrLine < 0)
						m_nCurrLine = 0;

					if(m_nCurrLine > m_nTotalLine)
						m_nCurrLine = m_nTotalLine;

					if( m_nTotalLine != 0)
						m_nCurrent = m_nCurrLine * 100 / m_nTotalLine;
					else
						m_nCurrent = 0;

					SetRegion();
				}

				// temp
				PostMessage( g_EngineInfo.m_hWnd, WM_XIAH_INTERFACE_MESSAGE, (WPARAM)m_nParentID, (LPARAM)m_nScrollSendID);
			}
		}
	}

	/**
	 * Draw
	 */
	void CUIScrollBar::Draw()
	{
		CUIControl::Draw();

		// TODO: 이것도 툴 재 작업시
	}

	/**
	 * SetVB
	 */
	void CUIScrollBar::SetVB()
	{
		if(!m_pVB)
			return;

		D3DCOLOR d3dcolor;

		if( m_nAlpha != 0)
			d3dcolor = D3DCOLOR_ARGB( m_nAlpha, 255, 255, 255);
		else
			d3dcolor = 0xffffffff;

		VT_TLVertex	Vertex[4];

		Vertex[ 0].pos = Vector4( m_rtPos.left,  m_rtPos.top, 0, 1);
		Vertex[ 1].pos = Vector4( m_rtPos.right, m_rtPos.top, 0, 1);
		Vertex[ 2].pos = Vector4( m_rtPos.left,  m_rtPos.bottom, 0, 1);
		Vertex[ 3].pos = Vector4( m_rtPos.right, m_rtPos.bottom, 0, 1);

		Vertex[ 0].diffuse = d3dcolor;
		Vertex[ 1].diffuse = d3dcolor;
		Vertex[ 2].diffuse = d3dcolor;
		Vertex[ 3].diffuse = d3dcolor;

		Vertex[ 0].tex = Vector2( 0, 0);
		Vertex[ 1].tex = Vector2( 1, 0);
		Vertex[ 2].tex = Vector2( 0, 1);
		Vertex[ 3].tex = Vector2( 1, 1);

		VOID* pVertices;
		if( !FAILED( m_pVB->Lock( 0, sizeof(Vertex), (void**)&pVertices, 0 )))
		{
			memcpy( pVertices, Vertex, sizeof(Vertex) );
			m_pVB->Unlock();
		}
	}

	/**
	 *
	 */
	void CUIScrollBar::SetRegion()
	{
		switch(m_nType2)
		{
		case SCROLL_HORIZONTAL_2:
			{
				m_rtPos.left = m_nMin + (float)( m_nMax - m_nMin) * ((float)m_nCurrent / 100.0);

				int BarLength = 20;

				if(m_nTotalLine)
					BarLength = (m_nMax - m_nMin) / m_nTotalLine;

				m_rtPos.right = m_rtPos.left + BarLength;

				if(m_rtPos.right > m_nMax)
				{
					m_rtPos.right = m_nMax;
					m_rtPos.left = m_nMax - BarLength;
				}
			}
			break;
		case SCROLL_VERTICAL_2:
			{
				m_rtPos.top = m_nMin + (float)( m_nMax - m_nMin) * ((float)m_nCurrent / 100.0);

				int BarLength = 20;

				//if( m_nTotalLine)
				//	BarLength = (m_nMax - m_nMin) / m_nTotalLine;

				//HT_0914 : 기연창 및 낭 아이템 개선 사항
				if( m_nTotalLine)
					BarLength += ((m_nMax - m_nMin) / m_nTotalLine);

				if(BarLength > 150)
					BarLength = 150;


				m_rtPos.bottom = m_rtPos.top + BarLength;

				if( m_rtPos.bottom > m_nMax)
				{
					m_rtPos.bottom = m_nMax;
					m_rtPos.top = m_nMax - BarLength;
				}
			}
			break;
		default:
			break;
		}

		SetVB();
	}

	/**
	 * 정보 변경
	 * \param data 변경 내용
	 */
	void CUIScrollBar::DataChange(sChangeData& data)
	{
		switch(data.nType)
		{
		case VALUE1:	// Scroll Bar Event Send ID
			m_nScrollSendID = data.nValue1;
			break;
		case TYPE:		// Scroll Type
			{
				m_nType2 = data.nValue1;

				switch(m_nType2)
				{
				case SCROLL_HORIZONTAL_2:
					m_nMin = m_rtPos.left;
					m_nMax = m_rtPos.right;
					break;
				case SCROLL_VERTICAL_2:
					m_nMin = m_rtPos.top;
					m_nMax = m_rtPos.bottom;
					break;
				default:
					break;
				}
			}
			break;
		case SCROLL_MAX:
			if(data.nValue1 >= 0)
				m_nMaxLine = data.nValue1;
			break;
		case SCROLL_TOTAL:
			if(data.nValue1 >= 0)
			{
				m_nTotalLine = data.nValue1;
				SetRegion();
			}
			break;
		case SCROLL_CURRENT:
			break;
		case SCROLL_MOVE:
			m_nCurrLine = data.nValue1;

			if(m_nCurrLine < 0)
				m_nCurrLine = 0;

			if(m_nCurrLine > m_nTotalLine)
				m_nCurrLine = m_nTotalLine;

			if(m_nTotalLine != 0)
				m_nCurrent = m_nCurrLine * 100 / m_nTotalLine;
			else
				m_nCurrent = 0;

			SetRegion();
			break;
		case SCROLL_ADD:
			m_nCurrLine += data.nValue1;

			if(m_nCurrLine < 0)
				m_nCurrLine = 0;

			if(m_nCurrLine > m_nTotalLine)
				m_nCurrLine = m_nTotalLine;

			if( m_nTotalLine != 0)
				m_nCurrent = m_nCurrLine * 100 / m_nTotalLine;
			else
				m_nCurrent = 0;

			SetRegion();
			break;
		default:
			CUIControl::DataChange(data);
			break;
		}
	}

	
	/**
	 * Get Data
	 * \param data 가져갈 정보
	 */
	void CUIScrollBar::GetData(sGetData& data)
	{
		switch(data.nType)
		{
		case GET_SCROLL_MAX:
			data.nValue1 = m_nMaxLine;
			break;
		case GET_SCROLL_TOTAL:
			data.nValue1 = m_nTotalLine;
			break;
		case GET_SCROLL_CURRENT:
			data.nValue1 = m_nCurrLine;
			break;
		default:
			CUIControl::GetData(data);
			break;
		}
	}
};

