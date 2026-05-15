/****************************************************************************************************
	파 일 명:   CUIProgressCtrl.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIProgressCtrl.h"

using namespace std;

namespace XiahGameEngine
{
	CUIProgressCtrl::CUIProgressCtrl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID),
																		m_nCurrentRatio(0), m_nProgressDir(0)
	{
	}

	CUIProgressCtrl::~CUIProgressCtrl()
	{
	}

	/**
	*
	* \param data 
	*/
	void CUIProgressCtrl::Create(sCtrlData& data)
	{
		CUIControl::Create(data);

		m_nMin = m_rtPos.left;
		m_nMax = m_rtPos.right;
	}

	/**
	*
	* \param mouse 
	*/
	void CUIProgressCtrl::MouseCheck(sMouseEvent& mouse)
	{
		//CUIControl::MouseCheck(mouse);
	}

	/**
	* Draw
	*/
	void CUIProgressCtrl::Draw()
	{
		// 차후 뒷배경 처리 될 부분

		// 앞부분 진행률 출력
		CUIControl::Draw();
	}

	/**
	* SetVB
	*/
	void CUIProgressCtrl::SetVB()
	{
		// 알파 정보가 있는것들은 알파를 준다
		D3DCOLOR d3dcolor;

		if( m_nAlpha != 0)
		{
			DWORD r = 255;//GetRValue( m_Color);
			DWORD g = 255;//GetGValue( m_Color);
			DWORD b = 255;//GetBValue( m_Color);

			d3dcolor = D3DCOLOR_ARGB( m_nAlpha, r, g ,b);
		}
		else
		{
			d3dcolor = 0xffffffff;
		}

		float fLeft		= m_rtPos.left;
		float fRight	= m_rtPos.right;
		float fTop		= m_rtPos.top;
		float fBottom	= m_rtPos.bottom;

		VT_TLVertex	Vertex[4];

		Vertex[ 0].pos = Vector4( fLeft, fTop, 0, 1);
		Vertex[ 1].pos = Vector4( fRight, fTop, 0, 1);
		Vertex[ 2].pos = Vector4( fLeft, fBottom, 0, 1);
		Vertex[ 3].pos = Vector4( fRight, fBottom, 0, 1);

		Vertex[ 0].diffuse = Vertex[ 1].diffuse = Vertex[ 2].diffuse = Vertex[ 3].diffuse = d3dcolor;

		switch(m_nProgressDir)
		{
		case LEFT_TO_RIGHT:
			{
				float fU = m_fU * (float)m_nCurrentRatio / 100.0;

				Vertex[ 0].tex = Vector2( 0,  0);
				Vertex[ 1].tex = Vector2( fU, 0);
				Vertex[ 2].tex = Vector2( 0,  m_fV);
				Vertex[ 3].tex = Vector2( fU, m_fV);
			}			
			break;
		case RIGHT_TO_LEFT: // 
			{
				float fU = m_fU - (m_fU * (float)m_nCurrentRatio / 100.0);

				Vertex[ 0].tex = Vector2( fU,	0);
				Vertex[ 1].tex = Vector2( m_fU, 0);
				Vertex[ 2].tex = Vector2( fU,	m_fV);
				Vertex[ 3].tex = Vector2( m_fU, m_fV);
			}			
			break;

		default:
			DBG_LogFile( _T("UI ProgressCtrl SetVB fail - [ Dir  %d ]"), m_nProgressDir);
			break;
		}

		if( m_pVB)
		{
			VOID* pVertices = NULL;

			if( !FAILED( m_pVB->Lock( 0, sizeof(Vertex), (void**)&pVertices, 0 )))
			{
				memcpy( pVertices, Vertex, sizeof(Vertex) );
				m_pVB->Unlock();
			}
			else
			{
				DBG_LogFile( _T("UI ProgressCtrl SetVB fail"));
			}
		}
	}

	/**
	* 정보 변경
	* \param data 변경 내용
	*/
	void CUIProgressCtrl::DataChange(sChangeData& data)
	{
		switch(data.nType)
		{
		case VALUE1: // 100% 비율로 설정
			{
				if(data.nValue1 >= 0 || data.nValue1 <= 100)
					m_nCurrentRatio = data.nValue1;
				else
					return;

				/////////////////////////////////////////////////////////////////////////////////////////////////////

				switch(m_nProgressDir)
				{
				case LEFT_TO_RIGHT:
					m_rtPos.right = m_rtPos.left + (float)( m_nMax - m_nMin) * ((float)m_nCurrentRatio / 100.0);
					break;
				case RIGHT_TO_LEFT:
					m_rtPos.left = m_rtPos.right - (float)( m_nMax - m_nMin) * ((float)m_nCurrentRatio / 100.0);
					break;
				}
			}
			break;

		case VALUE2: // min, max = 100%기준으로 설정
			{
				// nValue1 = min , nValue2 = max

				if(data.nValue2 == 0 || (data.nValue1 >= data.nValue2))
				{
					m_nCurrentRatio = 100;
				}
				else
				{
					if(data.nValue1 < 0)
						m_nCurrentRatio = 0;
					else
						m_nCurrentRatio	= (data.nValue1 / data.nValue2) * 100;
				}
			}			
			break;

		case TYPE: // ProgressDir
			{
				if(data.nValue1 == 0 || data.nValue1 == 1)
				{
					m_nProgressDir = data.nValue1;

					m_nMin = m_rtPos.left;
					m_nMax = m_rtPos.right;
				}
				else
				{
					DBG_LogFile( _T("UI ProgressCtrl DataChange fail - [ ProgressDir  %d ] ( 0 || 1 )"), data.nValue1);
					return;
				}
			}			
			break;

		default:
			{
				CUIControl::DataChange(data);
				return;
			}			
			break;
		} // switch(data.nType)

		SetVB();
	}
};

