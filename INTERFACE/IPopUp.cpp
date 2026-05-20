#include "StdAfx.h"
#include "IPopUp.h"
#include "../XiahPak.h"
#include "IExtern.h"


namespace XiahGameEngine
{
	XIAHGE_API CIPopUp::CIPopUp()
	{
		// popup 프레임 만들기
		m_pVB = NULL;
		g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex),
										D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX,
										D3DPOOL_MANAGED, &m_pVB, NULL);

		m_byGapLine = 2;

		m_FrameColor = D3DCOLOR_ARGB(0, 255, 255, 255);
	}

	XIAHGE_API CIPopUp::~CIPopUp()
	{
		if( m_pVB)
		{
			m_pVB->Release();
			m_pVB = NULL;
		}
	}

	/**
	 * 화면 출력
	 */
	XIAHGE_API void CIPopUp::Draw()
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

		for( int i=0; i < m_byLineCount; ++i)
		{
			m_Text2D[i].Render();
		}
	}
	

	/**
	 * 툴팁 설정
	 * \param byType 종류
	 * \param rtRect 
	 * \param byCount 
	 * \param ... 
	 */
	XIAHGE_API void CIPopUp::SetToolTip( BYTE byType, sRect* rtRect, BYTE byCount, ...)
	{
		if( byCount > MAXIMUM_LINE_COUNT-1 )
			return;

		// 어허, 이전에 그린 텍스트가 남아있구려,, 일단 간단하게 처리하자. 속도는 무시.
		for(int g=0; g < MAXIMUM_LINE_COUNT; ++g)
            m_Text2D[g].Clear();

		m_byLineCount = byCount;

		DWORD nAlign = TEXT2D_ALIGN_HLEFT;

		// 타입 종류에 따라 팝업의 위치 및 크기 변경
		switch( byType)
		{
		case 0:	// 아래로
			{
				m_rtFrameRect.left	= rtRect->left - 10;
				m_rtFrameRect.top	= rtRect->bottom;
				m_rtFrameRect.right = rtRect->left + 130 + 10;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 6:	// 무공툴팁1
			{
				m_rtFrameRect.left	= rtRect->left - 90;
				m_rtFrameRect.top	= rtRect->bottom;
				m_rtFrameRect.right = rtRect->right + 90;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 7:	// 무공툴팁2
			{
				m_rtFrameRect.left	= rtRect->left;
				m_rtFrameRect.top	= rtRect->bottom;
				m_rtFrameRect.right = rtRect->right + 200;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 5:	// 행낭창 5,6열을 위한 좌표
			{
				m_rtFrameRect.left	= rtRect->right - 130 - 20;
				m_rtFrameRect.top	= rtRect->bottom;
				m_rtFrameRect.right = rtRect->right;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 1:	// 위로
			{
				m_rtFrameRect.left	= rtRect->left;
				m_rtFrameRect.top	= rtRect->top - 12;
				m_rtFrameRect.right = rtRect->left + 130;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 2: // 오른쪽으로
			{
				m_rtFrameRect.left	= rtRect->right;
				m_rtFrameRect.top	= rtRect->top;
				m_rtFrameRect.right = rtRect->left + 130;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 3:	// 왼쪽으로
			{
				m_rtFrameRect.left	= rtRect->left - 100;
				m_rtFrameRect.top	= rtRect->top;
				m_rtFrameRect.right = rtRect->left;			
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 4:	// 자기 자신
			{
				m_rtFrameRect.left	= rtRect->left;
				m_rtFrameRect.top	= rtRect->top;
				m_rtFrameRect.right = rtRect->left;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
			break;
		case 8:
			{
				m_rtFrameRect.left	= rtRect->left;
				m_rtFrameRect.top	= rtRect->top;
				m_rtFrameRect.right = rtRect->right;
				nAlign				= DEFAULT_ALIGN;
				m_FrameColor		= D3DCOLOR_ARGB( 210, 50, 50, 50);
			}
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

		for( int i=0; i < byCount; ++i)
		{
			TCHAR* szTip = va_arg( ap, TCHAR*);
			
			if(	!szTip)
				szTip = _T("");

			m_rtTextRect[i].left	= m_rtFrameRect.left;
			m_rtTextRect[i].top		= m_rtFrameRect.top + VERTICAL_DISTANCE*i;
			m_rtTextRect[i].right	= m_rtFrameRect.right;
			m_rtTextRect[i].bottom	= m_rtTextRect[i].top + VERTICAL_DISTANCE;

			m_Text2D[i].SetParentRect( &m_rtTextRect[i]);

			D3DCOLOR color = va_arg( ap, D3DCOLOR);
			BYTE byFont = va_arg( ap, BYTE);

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
		
		SetVB(m_FrameColor);
	}

	/**
	 * 툴팁 추가
	 * \param szTip 출력 문자열
	 * \param color 색상
	 */
	XIAHGE_API void CIPopUp::AddToolTip( LPCTSTR szTip, D3DCOLOR color)
	{
		if( m_byLineCount >= MAXIMUM_LINE_COUNT - 1 ) return;
		++m_byLineCount;

		m_rtTextRect[ m_byLineCount -1] = m_rtTextRect[ m_byLineCount -2];

		int VerticalDistance = 0;

		// SetToolTip()에서 툴팁을 첨 만들고(이때에는 대부분 아템의 이름이므로 좀 크다.)
		// 이 함수에서는 추가적인 항목들을 툴팁으로 붙이는데, 이름과 그 밑에 항목들이
		// 약간의 간격을 두기 위해서 m_byLineCount가 2일때 간격을 키웠다. 
		// 그런데 "기연아이템"의 경우는 이름바로 밑에 (기연 아이템) 이라고 나와야 하고 
		// 그 담에 간격을 둬서 추가적인 항목들이 들어가야 한다. 그래서 이렇게 바꿈.
//		if( m_byLineCount == 2)
		if( m_byLineCount == m_byGapLine )
			VerticalDistance = 26;
		else
			VerticalDistance = 18;

		m_rtTextRect[ m_byLineCount -1].top += VerticalDistance;
		m_rtTextRect[ m_byLineCount -1].bottom += VerticalDistance;

		m_Text2D[ m_byLineCount -1].SetParentRect( &m_rtTextRect[ m_byLineCount -1]);
		m_Text2D[ m_byLineCount -1].SetText(  &m_rtTextRect[ m_byLineCount -1], DEFAULT_ALIGN, (LPCTSTR)szTip, DEFAULT_FONT, color);

		m_rtFrameRect.bottom = m_rtTextRect[ m_byLineCount -1].bottom;
		
		//D3DCOLOR FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
		SetVB(m_FrameColor);
	}

	/**
	 * 툴팁 추가
	 * \param szTip 출력 문자열
	 * \param byFontSize 폰트 크기
	 * \param color 색상
	 */
	XIAHGE_API void CIPopUp::AddToolTip( LPCTSTR szTip, BYTE byFontSize, D3DCOLOR color)
	{
		if( m_byLineCount >= MAXIMUM_LINE_COUNT - 1 ) return;
		++m_byLineCount;

		m_rtTextRect[ m_byLineCount -1] = m_rtTextRect[ m_byLineCount -2];

		int VerticalDistance = 0;

		// SetToolTip()에서 툴팁을 첨 만들고(이때에는 대부분 아템의 이름이므로 좀 크다.)
		// 이 함수에서는 추가적인 항목들을 툴팁으로 붙이는데, 이름과 그 밑에 항목들이
		// 약간의 간격을 두기 위해서 m_byLineCount가 2일때 간격을 키웠다. 
		// 그런데 "기연아이템"의 경우는 이름바로 밑에 (기연 아이템) 이라고 나와야 하고 
		// 그 담에 간격을 둬서 추가적인 항목들이 들어가야 한다. 그래서 이렇게 바꿈.
//		if( m_byLineCount == 2)
		if( m_byLineCount == m_byGapLine )
			VerticalDistance = 26;
		else
			VerticalDistance = 18;

		m_rtTextRect[ m_byLineCount -1].top += VerticalDistance;
		m_rtTextRect[ m_byLineCount -1].bottom += VerticalDistance;

		m_Text2D[ m_byLineCount -1].SetParentRect( &m_rtTextRect[ m_byLineCount -1]);
		m_Text2D[ m_byLineCount -1].SetText(  &m_rtTextRect[ m_byLineCount -1], DEFAULT_ALIGN, (LPCTSTR)szTip, GetFont( DEFAULT_FONT_NAME_2, byFontSize), color);

		m_rtFrameRect.bottom = m_rtTextRect[ m_byLineCount -1].bottom;
		
		//D3DCOLOR FrameColor = D3DCOLOR_ARGB( 210, 50, 50, 50);
		SetVB(m_FrameColor);
	}

	/**
	 * 정점 버퍼 설정
	 * \param FrameColor 색상
	 */
	void CIPopUp::SetVB( D3DCOLOR FrameColor)
	{
		// popup 프레임 만들기
		m_Vertex[ 0].pos = Vector4( m_rtFrameRect.left,  m_rtFrameRect.top,    0, 1);
		m_Vertex[ 1].pos = Vector4( m_rtFrameRect.right, m_rtFrameRect.top,    0, 1);
		m_Vertex[ 2].pos = Vector4( m_rtFrameRect.left,  m_rtFrameRect.bottom, 0, 1);
		m_Vertex[ 3].pos = Vector4( m_rtFrameRect.right, m_rtFrameRect.bottom, 0, 1);

		m_Vertex[ 0].diffuse = m_Vertex[ 1].diffuse = m_Vertex[ 2].diffuse = m_Vertex[ 3].diffuse = FrameColor;

		m_Vertex[ 0].tex = Vector2( 0, 0);
		m_Vertex[ 1].tex = Vector2( 1, 0);
		m_Vertex[ 2].tex = Vector2( 0, 1);
		m_Vertex[ 3].tex = Vector2( 1, 1);

		VOID* pVertices;

		if( m_pVB )
		{
			if( !FAILED( m_pVB->Lock( 0, sizeof(m_Vertex), (void**)&pVertices, 0 )))
			{
				memcpy( pVertices, m_Vertex, sizeof(m_Vertex) );
				m_pVB->Unlock();
			}
		}
		else
		{
			DBG_LogFile( _T("CIPopUp::SetVB fail"));
		}
	}

	XIAHGE_API void CIPopUp::SetGapLine(BYTE byGapLine)
	{
		m_byGapLine = byGapLine;
	}

	XIAHGE_API void CIPopUp::SetFrameColor(D3DCOLOR FrameColor)
	{
		m_FrameColor = FrameColor;

		SetVB(m_FrameColor);
	}
};