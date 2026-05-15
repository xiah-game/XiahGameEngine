/****************************************************************************************************
	파 일 명:   CUIToolTip.cpp
	만든날자:	2004/02/27  13:53
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIToolTip.h"

using namespace std;

namespace XiahGameEngine
{
	CUIToolTip::CUIToolTip(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIStaticText(pMeditatorRef,nID,nParentID)
	{
		// popup 프레임 만들기
		g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex),
			D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX,
			D3DPOOL_MANAGED, &m_pVB, NULL);

		m_byGapLine = 2;
	}

	CUIToolTip::~CUIToolTip()
	{
		if( m_pVB)
		{
			m_pVB->Release();
			m_pVB = NULL;
		}
	}

	void CUIToolTip::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}



	XIAHGE_API void CUIToolTip::Draw()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE);

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pDirect3DDevice->SetSamplerState( 0,  D3DSAMP_ADDRESSU , D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState( 0,  D3DSAMP_ADDRESSV , D3DTADDRESS_CLAMP);

		// 프레임 그리기
		g_Device.SetTexture(0, NULL);
		//g_pDirect3DDevice->SetTexture( 0, NULL );
		g_Device.SetStreamSource( m_pVB, sizeof(VT_TLVertex));
		g_Device.SetFVF(D3DFVF_TLVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX);
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

		for( int i=0; i<m_byLineCount; i++)
		{
			m_Text2D[i].Render();
		}
	}

	//=
	// 툴팁 설정
	// @param byType 종류
	// @param rtRect 
	//=
	XIAHGE_API void CUIToolTip::SetToolTip( BYTE byType, sRect* rtRect, BYTE byCount, ...)
	{
		if( byCount > MAXIMUM_LINE_COUNT_2-1 )
			return;

		m_byLineCount = byCount;
		D3DCOLOR color;
		BYTE byFont;
		DWORD nAlign = TEXT2D_ALIGN_HLEFT;
		D3DCOLOR FrameColor = D3DCOLOR_ARGB( 0, 255, 255, 255);

		// 타입 종류에 따라 팝업의 위치 및 크기 변경
		switch( byType)
		{
		case 0:	// 아래로
			m_rtFrameRect.left = rtRect->left;
			m_rtFrameRect.top = rtRect->bottom; 
			m_rtFrameRect.right = rtRect->left + 130;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 6:	// 무공툴팁1
			m_rtFrameRect.left = rtRect->left - 100;
			m_rtFrameRect.top = rtRect->bottom;
			m_rtFrameRect.right = rtRect->right + 100;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 7:	// 무공툴팁2
			m_rtFrameRect.left = rtRect->left;
			m_rtFrameRect.top = rtRect->bottom;
			m_rtFrameRect.right = rtRect->right + 200;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 5:	// 행낭창 5,6열을 위한 좌표
			m_rtFrameRect.left = rtRect->right - 130;
			m_rtFrameRect.top = rtRect->bottom;
			m_rtFrameRect.right = rtRect->right;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 1:	// 위로
			m_rtFrameRect.left = rtRect->left;
			m_rtFrameRect.top = rtRect->top - 12;
			m_rtFrameRect.right = rtRect->left + 130;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 2: // 오른쪽으로
			m_rtFrameRect.left = rtRect->right;
			m_rtFrameRect.top = rtRect->top;
			m_rtFrameRect.right = rtRect->left + 130;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 3:	// 왼쪽으로
			m_rtFrameRect.left = rtRect->left - 100;
			m_rtFrameRect.top = rtRect->top;
			m_rtFrameRect.right = rtRect->left;			
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		case 4:	// 자기 자신
			m_rtFrameRect.left = rtRect->left;
			m_rtFrameRect.top = rtRect->top;
			m_rtFrameRect.right = rtRect->left;
			nAlign = DEFAULT_ALIGN;
			FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
			break;
		}
		if(m_rtFrameRect.right >= 1024)
		{
			LONG nTemp = 0;

			nTemp = m_rtFrameRect.right - 1024;

			m_rtFrameRect.left = m_rtFrameRect.left - nTemp;
			m_rtFrameRect.right = m_rtFrameRect.right - nTemp;
		}


		// popup 글자 세팅
		va_list ap;
		va_start( ap, byCount);

		for( int i=0; i<byCount; i++)
		{
			TCHAR* szTip = va_arg( ap, TCHAR*);

			if(	!szTip)
				szTip = _T("");

			m_rtTextRect[i].left = m_rtFrameRect.left;
			m_rtTextRect[i].top = m_rtFrameRect.top + VERTICAL_DISTANCE_2*i;
			m_rtTextRect[i].right = m_rtFrameRect.right;
			m_rtTextRect[i].bottom = m_rtTextRect[i].top + VERTICAL_DISTANCE_2;

			m_Text2D[i].SetParentRect( &m_rtTextRect[i]);

			color = va_arg( ap, D3DCOLOR);
			byFont = va_arg( ap, BYTE);

			switch( byFont)
			{
			case 0:
				m_Text2D[i].SetText(  &m_rtTextRect[i], nAlign, (LPCTSTR)szTip, SMALL_FONT, color);
				break;
			case 1:
				m_Text2D[i].SetText( &m_rtTextRect[i], nAlign, (LPCTSTR)szTip, ITEMNAME_FONT, color);
				break;
			case 2:
				m_Text2D[i].SetText( &m_rtTextRect[i], nAlign, (LPCTSTR)szTip, DEFAULT_FONT, color);
				break;
			default:
				m_Text2D[i].SetText( 0, 0, (LPCTSTR)szTip, SMALL_FONT, color);
				break;
			}
		}

		m_rtFrameRect.bottom = m_rtTextRect[ byCount-1].bottom;


		SetVB( FrameColor);
	}

	//=
	// 툴팁 추가
	// @param szTip 출력 문자열
	// @param color 색상
	//=
	XIAHGE_API void CUIToolTip::AddToolTip( LPCTSTR szTip, D3DCOLOR color)
	{
		m_byLineCount++;

		m_rtTextRect[ m_byLineCount -1] = m_rtTextRect[ m_byLineCount -2];

		int VerticalDistance = 0;

		if( m_byLineCount == m_byGapLine )
			VerticalDistance = 26;
		else
			VerticalDistance = 18;

		m_rtTextRect[ m_byLineCount -1].top += VerticalDistance;
		m_rtTextRect[ m_byLineCount -1].bottom += VerticalDistance;

		m_Text2D[ m_byLineCount -1].SetParentRect( &m_rtTextRect[ m_byLineCount -1]);
		m_Text2D[ m_byLineCount -1].SetText(  &m_rtTextRect[ m_byLineCount -1], DEFAULT_ALIGN, (LPCTSTR)szTip, DEFAULT_FONT, color);

		m_rtFrameRect.bottom = m_rtTextRect[ m_byLineCount -1].bottom;

		D3DCOLOR FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
		SetVB( FrameColor);
	}

	//=
	// 툴팁 추가
	// @param szTip 출력 문자열
	// @param byFontSize 폰트 크기
	// @param color 색상
	//=
	XIAHGE_API void CUIToolTip::AddToolTip( LPCTSTR szTip, BYTE byFontSize, D3DCOLOR color)
	{
		++m_byLineCount;

		m_rtTextRect[ m_byLineCount -1] = m_rtTextRect[ m_byLineCount -2];

		int VerticalDistance = 0;

		if( m_byLineCount == m_byGapLine )
			VerticalDistance = 26;
		else
			VerticalDistance = 18;

		m_rtTextRect[ m_byLineCount -1].top += VerticalDistance;
		m_rtTextRect[ m_byLineCount -1].bottom += VerticalDistance;

		m_Text2D[ m_byLineCount -1].SetParentRect( &m_rtTextRect[ m_byLineCount -1]);
		m_Text2D[ m_byLineCount -1].SetText(  &m_rtTextRect[ m_byLineCount -1], DEFAULT_ALIGN, (LPCTSTR)szTip, GetFont( DEFAULT_FONT_NAME_2, byFontSize), color);

		m_rtFrameRect.bottom = m_rtTextRect[ m_byLineCount -1].bottom;

		D3DCOLOR FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
		SetVB( FrameColor);
	}

	//=
	// 정점 버퍼 설정
	// @param FrameColor 색상
	//=
	void CUIToolTip::SetVB( D3DCOLOR FrameColor)
	{
		// popup 프레임 만들기
		D3DCOLOR d3dcolor = FrameColor;

		m_Vertex[ 0].pos = Vector4( m_rtFrameRect.left,  m_rtFrameRect.top,    0, 1);
		m_Vertex[ 1].pos = Vector4( m_rtFrameRect.right, m_rtFrameRect.top,    0, 1);
		m_Vertex[ 2].pos = Vector4( m_rtFrameRect.left,  m_rtFrameRect.bottom, 0, 1);
		m_Vertex[ 3].pos = Vector4( m_rtFrameRect.right, m_rtFrameRect.bottom, 0, 1);

		m_Vertex[ 0].diffuse = d3dcolor;
		m_Vertex[ 1].diffuse = d3dcolor;
		m_Vertex[ 2].diffuse = d3dcolor;
		m_Vertex[ 3].diffuse = d3dcolor;

		m_Vertex[ 0].tex = Vector2( 0, 0);
		m_Vertex[ 1].tex = Vector2( 1, 0);
		m_Vertex[ 2].tex = Vector2( 0, 1);
		m_Vertex[ 3].tex = Vector2( 1, 1);

		VOID* pVertices;

		if( !FAILED( m_pVB->Lock( 0, sizeof(m_Vertex), (void**)&pVertices, 0 )))
		{
			memcpy( pVertices, m_Vertex, sizeof(m_Vertex) );
			m_pVB->Unlock();
		}
		else
		{
			DBG_LogFile( _T("CIPopUp::SetVB fail"));
		}
	}

	XIAHGE_API void CUIToolTip::SetGapLine(BYTE byGapLine)
	{
		m_byGapLine = byGapLine;
	}
};

