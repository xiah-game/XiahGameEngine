/****************************************************************************************************
	파 일 명:   CUIControl.cpp
	만든날자:	2004/02/21  10:58
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIControl.h"
#include "../XiahPak.h"

#include "CUIBasisDialog.h"

using namespace std;

namespace XiahGameEngine
{
	extern CUIToolTip*			g_pToolTip_2;
	extern bool g_bTip_2;

	CUIControl::CUIControl(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : m_nType(0), m_nType2(0), m_bShow(true), m_nResID(0),
	m_nAlpha(0), m_pVB(NULL), m_pTexture(NULL), m_pText2D(NULL), m_pMediatorRef(pMeditatorRef), m_nID(nID), m_nParentID(nParentID), m_bActive(false)
	{
		// temp
		m_bOnShowToolTip = FALSE;
		m_sToolTipText = _T("");
		m_byToolTipType = m_byToolTipFontType = m_byAddedToolTipCount = 0;
		
		for(register int i=0; i < 20; ++i)
		{
			m_sToolTipTextAdded[ i] = _T("");
			m_byToolTipAddedFontType[ i] = 0;
		}

		m_pText2D = new CText2D;
	}

	CUIControl::~CUIControl()
	{
		delete m_pText2D, m_pText2D = NULL;
		//Destroy();
	}

	/**
	* Create
	* \param data 생성 정보
	*/
	void CUIControl::Create(sCtrlData& data)
	{
		m_nID		= data.nID;
		m_nType		= data.nType;
		m_nResID	= data.nResID;
		m_nAlpha	= data.nAlpha;

		m_rtPos.left	= data.nX;
		m_rtPos.top		= data.nY;
		m_rtPos.right	= data.nWidth;
		m_rtPos.bottom	= data.nHeight;

		m_fU = data.fU;
		m_fV = data.fV;

		CreateVB();
	}

	void CUIControl::ReCreate()
	{
		CreateVB();
	}

	/**
	* 해제
	*/
	void CUIControl::Destroy()
	{		
		XiahPak::ReleaseRes( m_nResID);
		m_pTexture = NULL;

		if(m_pVB != NULL)
			m_pVB->Release(), m_pVB = NULL;

		//delete m_pText2D, m_pText2D = NULL;
	}

	/**
	*
	*/
	void CUIControl::Changed()
	{
		if(m_pMediatorRef)
			m_pMediatorRef->ControlChanged(this, m_nID);
	}

	/**
	* 마우스 검사
	* \param mouse 마우스 이벤트 정보
	*/
	//HT_CHEAT : WINDOWSIZE
	void CUIControl::MouseCheck(sMouseEvent& mouse)
	{
		if(!m_bShow)
			return;

		sPoint ptMouse(mouse.ptMousePos.x,mouse.ptMousePos.y);		
		
		if( m_rtPos.PtInRect(ptMouse) ) //PtInRect(&m_rtPos, mouse.ptMousePos))
		{
			mouse.nEventType	= 1;
			mouse.nEventFrameID = m_nParentID;
			mouse.nEventCtrlID	= m_nID;

			/////////////////////////////////////////////////////////////////////////////////////////////////////
			// TEMP 왜 시아는 초기 이런구조로 했는가...
			// 툴팁 찍기
			if( m_sToolTipText !=_T(""))
			{
				g_pToolTip_2->SetToolTip( m_byToolTipType, &m_rtPos, 1, (LPCTSTR)m_sToolTipText, DEFAULT_COLOR, m_byToolTipFontType);

				m_bOnShowToolTip = TRUE;
				g_bTip_2 = true;

				for(register int i=0; i < m_byAddedToolTipCount; ++i)
				{
					if( m_sToolTipTextAdded[ i] != _T(""))
					{
						D3DCOLOR color;
						switch( m_byToolTipAddedFontType[ i])
						{
						case 1:
							color = D3DCOLOR_XRGB( 255, 255, 0);
							break;
						case 2:
							color = D3DCOLOR_XRGB( 255, 0, 0);
							break;
						default:
							color = D3DCOLOR_XRGB( 255, 255, 255);
							break;
						}

						g_pToolTip_2->AddToolTip( (LPCTSTR)m_sToolTipTextAdded[ i], color);
					}
				}
			}

		} // if(PtInRect(&m_rtPos, mouse.ptMousePos))
		else if(m_bOnShowToolTip)
		{
			m_bOnShowToolTip = FALSE;
			g_bTip_2 = false;
		}
	}

	/**
	* Set Pos
	* \param nPosX X좌표
	* \param nPosY Y좌표
	*/
	void CUIControl::SetPos(const int nPosX, const int nPosY)
	{
		int nDiffX = nPosX - m_rtPos.left;
		int nDiffY = nPosY - m_rtPos.top;

		m_rtPos.left	+= nDiffX;
		m_rtPos.right	+= nDiffX;
		m_rtPos.top		+= nDiffY;
		m_rtPos.bottom	+= nDiffY;

		SetVB();
	}

	/**
	* Set Move Pos
	* \param nMoveX X
	* \param nMoveY Y
	*/
	void CUIControl::SetMovePos(const int nMoveX, const int nMoveY)
	{
		m_rtPos.left	+= nMoveX;
		m_rtPos.right	+= nMoveX;
		m_rtPos.top		+= nMoveY;
		m_rtPos.bottom	+= nMoveY;

		SetVB();
	}

	/**
	 * Set Pos Diff Return
	 * \param &nPosX 
	 * \param &nPosY 
	 */
	void CUIControl::SetPosDiffReturn(int &nPosX, int &nPosY)
	{
		nPosX = nPosX - m_rtPos.left;
		nPosY = nPosY - m_rtPos.top;

		m_rtPos.left	+= nPosX;
		m_rtPos.right	+= nPosX;
		m_rtPos.top		+= nPosY;
		m_rtPos.bottom	+= nPosY;

		SetVB();
	}


	/**
	 * Draw
	 */
	void CUIControl::Draw()
	{
		if(!m_bShow)
		{
			return;
		}
		else
		{
			if(m_nAlpha != 0)
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			}

			g_Device.SetTexture(0, m_pTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pTexture );
			g_Device.SetStreamSource( m_pVB, sizeof(VT_TLVertex));
			g_Device.SetFVF(D3DFVF_TLVERTEX);
			//g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX);
			g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		}
	}

	/**
	* 문자열 설정
	* \param szText 텍스트
	* \param pFont 폰트
	*/
	void CUIControl::SetString(LPCTSTR szText, sFont* pFont)
	{
		//if( !m_pText2D)
		//	m_pText2D = new CText2D;

		m_pText2D->Clear();

		m_pText2D->SetParentRect(&m_rtPos);
		m_pText2D->SetText(&m_rtPos.GetLocal(), DEFAULT_ALIGN, (LPCTSTR)szText, pFont, DEFAULT_COLOR);
	}

	/**
	* 문자열 설정
	* \param szText 텍스트
	* \param byType 유형
	*/
	void CUIControl::SetString(LPCTSTR szText, byte byType)
	{
//		if(!m_pText2D)
//		{
//			m_pText2D = new CText2D;
//		}

		if(_tcslen( szText) < 1)
		{
			m_pText2D->Clear();
		}
		else
		{
			m_pText2D->SetParentRect( &m_rtPos);

			switch(byType)
			{
			case 0:  // 연노랑
				m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 255, 255));
				break;

			case 1: // 빨간색
				m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 0, 0));
				break;
			case 2:
				m_pText2D->SetText(  &m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 0, 0));
				break;
			case 3:	
				m_pText2D->SetText(  &m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 255, 0));
				break;
			case 4:
				m_pText2D->SetText(  &m_rtPos.GetLocal(), TEXT2D_ALIGN_HRIGHT,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB(255, 255, 255));
				break;
			case 5:
				m_pText2D->SetText(  &m_rtPos.GetLocal(), TEXT2D_ALIGN_HLEFT,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB(255, 255, 255));
				break;
			case 6:	// 회색
				m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
					(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB(180, 180, 180));
				break;
			case 7:	// 옥션용
				{
					m_pText2D->SetText(&m_rtPos.GetLocal(), DEFAULT_ALIGN,
						(LPCTSTR)szText, GetFont( _T("若뗤퐪"), 15), D3DCOLOR_XRGB(255, 0, 0));
				}
				break;
			case 8:	// 옵션 금전
				{
					m_pText2D->SetText(&m_rtPos.GetLocal(), TEXT2D_ALIGN_VCENTER | TEXT2D_ALIGN_HRIGHT,
						(LPCTSTR)szText, GetFont(_T("若뗤퐪"), 12), D3DCOLOR_XRGB(255, 255, 255));
				}
				break;
			case 9:	//금전 100,000(일십만) 단위	노란색
				{
                    m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
                        (LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 255, 0));
				}
                break;
			case 10: //금전 1,000,000(일백만) 단위 밝은녹색
				{
                    m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
                        (LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 0, 255, 0));
				}
				break;
			case 11: //금전 10,000,000(일천만) 단위 하늘색
				{
                    m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
                        (LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 0, 204, 255));
				}
				break;
			case 12: //금전 100,000,000(일억) 단위 분홍색
				{
                    m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
                        (LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 0, 255));
				}
				break;
			case 13: //금전 1,000,000,000(일십억) 단위 연한 주황색
				{
                    m_pText2D->SetText(  &m_rtPos.GetLocal(), DEFAULT_ALIGN,
                        (LPCTSTR)szText, GetFont( _T("若뗤퐪"), 12), D3DCOLOR_XRGB( 255, 153, 0));
				}
			}
		}
	}

	/**
	* 문자열 설정 (숫자)
	* \param nNum 설정될 숫자
	*/
	void CUIControl::SetString(const int nNum, const int nType)
	{
		sString szText;
		szText.printf( _T("%d"), nNum);

//		if( !m_pText2D)
//		{
//			m_pText2D = new CText2D;
//		}

		m_pText2D->SetParentRect(&m_rtPos);

		DWORD dwType = DEFAULT_ALIGN;

		switch(nType)
		{
		case 0:
			dwType = DEFAULT_ALIGN;
			break;
		case 1:
			dwType = TEXT2D_ALIGN_HLEFT;
			break;
		case 2:
			dwType = TEXT2D_ALIGN_HRIGHT;
			break;
		default:
			dwType = DEFAULT_ALIGN;
			break;
		} // switch(nType)

		m_pText2D->SetText(&m_rtPos.GetLocal(), dwType, (LPCTSTR)szText, DEFAULT_FONT, DEFAULT_COLOR);
	}


	/**
	 * CreateVB
	 */
	void CUIControl::CreateVB()
	{
		// VB
		g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex), 0, D3DFVF_TLVERTEX, D3DPOOL_MANAGED, &m_pVB, NULL);

		SetVB();

		// Tex
		if(m_nResID)
			m_pTexture = XiahPak::GetTexture( m_nResID, TRUE);
	}

	/**
	* SetVB
	*/
	void CUIControl::SetVB()
	{
		if(!m_pVB)
			return;

		D3DCOLOR d3dcolor;

		if(m_nAlpha != 0)
		{
			register DWORD r;//GetRValue( m_Color);
			//DWORD g;//GetGValue( m_Color);
			//DWORD b;//GetBValue( m_Color);

			if(m_nType == CTRL_FRAME) // frame
			{
				r = 0;//g = b = 0;//GetRValue( m_Color);
			}
			else // control
			{
				r = 255; //g = b = 255;//GetRValue( m_Color);
			}			

			d3dcolor = D3DCOLOR_ARGB(m_nAlpha, r, r ,r);
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
	* 정보 변경
	* \param data 변경 내용
	*/
	void CUIControl::DataChange(sChangeData& data)
	{
		switch(data.nType)
		{
		// 문자열 설정
		case STRING0:
			SetString(data.strText, data.pFont);
			break;
		case STRING1:
			SetString(data.strText, data.nValue1);
			break;
		case STRING2:
			SetString(data.nValue1, data.nValue2);
			break;

		case 51: // 이동수치로 설정
			{
				SetMovePos(data.nValue1, data.nValue2);
			}			
			break;
		case 50: // 이동 위치로 설정
			{
				SetPosDiffReturn(data.nValue1, data.nValue2);
				data.nType = 51; // 하위 컨트롤을 위해
			}			
			break;			
		case COLOR:
			{
				SetColor(data.dwPtr);
			}
			break;
		case TEXTURE: // Set Texture
			{
				m_nResID = data.nValue1;
				m_pTexture = XiahPak::GetTexture(m_nResID, true);
			}			
			break;
		case SET_TOOLTIP:
			SetToolTip(data.nValue1, data.strText, data.nValue2);
			break;
		case ADD_TOOLTIP_1:
			AddToolTip(data.strText, data.nValue1);
			break;
		case ADD_TOOLTIP_2:
			AddToolTip(data.nValue1, data.strText, data.nValue2);
			break;
		}
	}


	/**
	 * Get Data
	 * \param data 가져올 정보
	 */
	void CUIControl::GetData(sGetData& data)
	{
		switch(data.nType)
		{
		case GET_REGION:		// Region
			{
				memcpy(&data.rtData, &m_rtPos, sizeof(RECT));
			}			
			break;
		case GET_IS_MOUSEON:	// IsMouseOn
			{
				if(m_bShow)
				{
					//HT_CHEAT : WINDOWSIZE
					sPoint ptMouse(data.ptMousePos.x,data.ptMousePos.y);
					if( m_rtPos.PtInRect(ptMouse) )
						data.bEvent = true;
				}
			}
			break;
		case GET_IS_FOCUS:		// IsFocus
			{
				data.bEvent = m_bActive;
			}			
			break;
		case GET_TEXTURE:		// Texture ID (Res ID)
			{
				data.nValue1 = m_nResID;
			}			
			break;
		case GET_IS_SHOW:
			{
				data.bEvent = m_bShow;
			}
			break;
		}		
	}

	void CUIControl::SetColor(DWORD dwColor)
	{
		if(m_pText2D)
			m_pText2D->SetColor(dwColor);
	}

    /////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// TEMP
	/**
	 * 툴팁 설정
	 * \param byToolTipType 툴팁 유형
	 * \param sToolTipText 툴팁 텍스트
	 * \param byToolTipFontType 폰트 유형
	 */
	void CUIControl::SetToolTip(BYTE byToolTipType, LPCTSTR sToolTipText, BYTE byToolTipFontType)
	{ 
		m_byToolTipType = byToolTipType; 
		m_sToolTipText = sToolTipText;
		m_byToolTipFontType = byToolTipFontType;

		// 여기서 기존 툴팁은 모두 없앤다
		for(register int i=0; i < 20; ++i)
		{
			m_sToolTipTextAdded[i] = _T("");
			m_byToolTipAddedFontType[i] = 0;
		}

		m_byAddedToolTipCount = 0;
	}

	/**
	 * 툴팁 추가
	 * \param sToolTipText 텍스트
	 * \param byFontType 폰트 유형
	 */
	void CUIControl::AddToolTip(LPCTSTR sToolTipText, BYTE byFontType)
	{
		for(register int i=0; i < 20; ++i)
		{
			if( m_sToolTipTextAdded[ i] == _T(""))
			{
				m_sToolTipTextAdded[ i] = sToolTipText;
				m_byToolTipAddedFontType[ i] = byFontType;
				++m_byAddedToolTipCount;
				return;
			}
		}
	}

	/**
	 * 툴팁 추가
	 * \param byIndex 추가 위치
	 * \param sToolTipText 텍스트
	 * \param byFontType 유형
	 */
	void CUIControl::AddToolTip(BYTE byIndex, LPCTSTR sToolTipText, BYTE byFontType)
	{
		m_sToolTipTextAdded[byIndex] = sToolTipText;
		m_byToolTipAddedFontType[byIndex] = byFontType;
		++m_byAddedToolTipCount;
	}
};