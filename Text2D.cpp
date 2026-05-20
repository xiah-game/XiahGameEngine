#include "stdafx.h"
#include "D3DDevice.h"
#include "Text2D.h"
#include "io.h"

#include <assert.h>

namespace XiahGameEngine
{
	//---------------------------------------------------------------------------------------
	// MULTI LINE SUPPORT
	//---------------------------------------------------------------------------------------

	XIAHGE_API CText2D::CText2D()
	{
		m_howmanylines = 0;
		m_skip = 3;

		m_text2d = new CText2D_s[MAX_MULTILINES];
		for(int i=0; i < MAX_MULTILINES; ++i)
		{
			memset(m_message[i], 0, sizeof(m_message[i]));
			m_Align[i] = 0;
		}
	}

	XIAHGE_API CText2D::~CText2D()
	{
		delete [] m_text2d;
		m_text2d = NULL;
	}

	XIAHGE_API BOOL	CText2D::Clear()
	{
		m_howmanylines = 0;
		
		// CText2D_s 객체 지웠다 다시 만든다.		
		delete [] m_text2d;
		m_text2d = NULL;

		m_text2d = new CText2D_s[MAX_MULTILINES];		
		
		if(m_text2d == NULL) 
			return FALSE;

		for(int i=0; i < MAX_MULTILINES; ++i)
		{
			memset(m_message[i], 0, sizeof(m_message[i]));
			//m_Align[i] = 0;
		}

		return TRUE;
	}

	XIAHGE_API void CText2D ::Set_SkipLine(int skip)
	{
		m_skip = skip;
	}

	XIAHGE_API void CText2D ::Calc_Lines(LPCTSTR str)
	{
		TCHAR buf[512] = {0,};
		int c = 0;
		m_howmanylines = 0;

		int len = _tcslen(str);
		for(int i=0; i < len; ++i)
		{
			if(str[i] == SEPARATE_MARK)
			{
				if(c > 0 && i != len-1)
				{
					if (m_howmanylines < MAX_MULTILINES) {
						_tcscpy(m_message[m_howmanylines++],buf);
					}
					c = 0;
					memset(buf,0,sizeof(buf));
				}
			}
			else
			{
				if (IsDBCSLeadByte(str[i])) {
					if (c < 62) {
						buf[c++] = (TCHAR)str[i];
						if (i + 1 < len) {
							buf[c++] = (TCHAR)str[++i];
						}
					} else {
						// Not enough room for a 2-byte character, skip it
						if (i + 1 < len) i++;
					}
				} else {
					if (c < 63) {
						buf[c++] = (TCHAR)str[i];
					}
				}
			}
		}

		if (m_howmanylines < MAX_MULTILINES) {
			_tcscpy(m_message[m_howmanylines++],buf);
		}
	}

	XIAHGE_API BOOL CText2D::SetAlign(DWORD *align,int num)
	{
		if(num > MAX_MULTILINES)
			return FALSE;

		for(register int i=0; i < num; ++i)
		{
			m_Align[i] = align[i];
		}

		return TRUE;
	}

	XIAHGE_API DWORD CText2D::GetAlign()
	{
		DWORD count = 0;
		for(register int i=0; i < MAX_MULTILINES; ++i)
		{
			if(m_Align[i])
				++count;
		}
		return count;
	}

	// 몇라인짜리 텍스트인가??
	XIAHGE_API DWORD CText2D::GetLineNum()
	{
		return m_howmanylines;
	}

	XIAHGE_API BOOL CText2D::SetText(sRect *pRect,DWORD nAlign,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color)
	{
		if(str == NULL)
		{
			return false;		    
		}

		BOOL UseAlign = FALSE;
		int height = 0;
		Calc_Lines(str);

		if(GetAlign() > 0)
		{
			UseAlign = TRUE;
		}

		for(int i=0; i < m_howmanylines; ++i)
		{
			// 수동 정렬을 사용한다.
			if(UseAlign==TRUE)
				nAlign = m_Align[i];

			if(m_text2d[i].SetText(pRect,nAlign,m_message[i],pFont,color,outline_color) == FALSE) return TRUE;

			if(i > 0)
			{
				m_text2d[i].m_Rect.top += height;	// Line Skip
				m_text2d[i].m_Rect.bottom += height;
			}

			height += m_text2d[i].m_Rect.bottom - m_text2d[i].m_Rect.top + m_skip;
		}
		return TRUE;
	}

	XIAHGE_API BOOL CText2D::SetText(int x,int y,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color,int nColorCount, ...)
	{
		int height = 0;
		Calc_Lines(str);
		
		va_list ap;

		if(nColorCount > 0)
			va_start( ap, nColorCount);
		
		//D3DCOLOR dwColor = va_arg( ap, D3DCOLOR);

		for(int i=0; i < m_howmanylines; ++i)
		{
			if(nColorCount > 0 && nColorCount >= i && i != 0)
			{
				color = va_arg( ap, D3DCOLOR);
			}

			if(m_text2d[i].SetText(x,y,m_message[i],pFont,color,outline_color) == false)
				goto LineSkip;  // 하위 메시지 라인도 있으면 검사 해주어야 하기에 아래로 이동시켜야 함.

			if(i>0)
			{
				m_text2d[i].m_Rect.top += height;	// Line Skip
				m_text2d[i].m_Rect.bottom += height;
			} // if(i>0)

LineSkip:
			height += m_text2d[i].m_Rect.Height() + m_skip;
		} // for(int i=0;i<m_howmanylines;i++)

		if(nColorCount > 0)
			va_end(ap);

		return TRUE;
	}

	XIAHGE_API BOOL CText2D::Render(float fix_x,float fix_y)
	{
		for(register int i=0; i < m_howmanylines; ++i)
		{
			m_text2d[i].Render(fix_x,fix_y);
		}
		return TRUE;
	}

	XIAHGE_API BOOL CText2D::Release()
	{
		for(register int i=0; i < MAX_MULTILINES; ++i)
			m_text2d[i].Release();

		return TRUE;
	}

	XIAHGE_API BOOL CText2D::SetParentRect(sRect *pParentRect)
	{
		for(register int i=0; i < MAX_MULTILINES; ++i)
			m_text2d[i].SetParentRect(pParentRect);

		return TRUE;
	}

	XIAHGE_API void CText2D::SetColor(D3DCOLOR dwColor)
	{
		for(int i=0; i < m_howmanylines; ++i)
		{
			m_text2d[i].SetColor(dwColor);
		}
	}

	/*
	XIAHGE_API sSize CText2D::GetSize()
	{ 
		return sSize(m_text2d[0].m_Rect.Width(),m_text2d[0].m_Rect.Height());
	}
	*/

	/*
	XIAHGE_API sSize CText2D::GetSize(int nNum)
	{
		if(nNum >= MAX_MULTILINES)
			return sSize(0,0);

		return sSize(m_text2d[nNum].m_Rect.Width(),m_text2d[nNum].m_Rect.Height());
	}
	*/

#define GetMonochromBit(pData, index)	((*((pData) + ((index) / 8)) >> (8 - ((index) % 8)- 1)) & 0x1)

	BYTE		CText2D_s::m_cBuffer[ 1024];
	unsigned short  g_TextTempBuffer[ 512 * 1024];
	unsigned short* g_TextBufferTemp[ 1024 / 256];

	//---------------------------------------------------------------------------------------
	XIAHGE_API CText2D_s::CText2D_s()
	{
		m_pFont = NULL;
		m_bVisible = TRUE;
		m_nFace = 0;
		m_pParentRect = NULL;

		g_pDirect3DDevice->CreateVertexBuffer( 4*4*sizeof(VT_TLVertex), D3DUSAGE_WRITEONLY ,D3DFVF_TLVERTEX, D3DPOOL_MANAGED, &m_VB, NULL );
	}

	//---------------------------------------------------------------------------------------
	XIAHGE_API CText2D_s::~CText2D_s()
	{
		Release();
	}

	XIAHGE_API BOOL CText2D_s::Release()
	{
		if( m_VB )
			m_VB->Release();
		m_VB = NULL;

		ReleaseTexture();
		return TRUE;
	}

	//---------------------------------------------------------------------------------------

	XIAHGE_API BOOL CText2D_s::SetText(sRect *pRect,DWORD nAlign,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color)
	{

		if( m_String == str && m_pFont == pFont && m_Color == color)
			return FALSE;
	
		if( !SetText( pRect->left, pRect->top, str, pFont, color, outline_color))
			return FALSE;

		int text_xsize = m_Rect.Width();
		int text_ysize = m_Rect.Height();

		int x_pos	= pRect->left;
		int y_pos	= pRect->top;

		switch( nAlign & 0x000FFF)
		{
		case TEXT2D_ALIGN_HLEFT:
			x_pos = 0;
			break;
		case TEXT2D_ALIGN_HRIGHT:
			x_pos = pRect->Width() - text_xsize;
			break;
		case TEXT2D_ALIGN_HCENTER:
			x_pos = (pRect->Width() - text_xsize) / 2;
			break;
		}
		
		switch( nAlign & 0xFFF000)
		{
		case TEXT2D_ALIGN_VTOP:
			y_pos = 0;
			break;
		case TEXT2D_ALIGN_VBOTTOM:
			y_pos = pRect->Height() - text_ysize;
			break;
		case TEXT2D_ALIGN_VCENTER:
			y_pos = (pRect->Height() - text_ysize) / 2;
			break;
		}

		m_Rect.left = x_pos;
		m_Rect.top = y_pos;
		m_Rect.right = m_Rect.left + text_xsize;
		m_Rect.bottom = m_Rect.top + text_ysize;

		PrepareRender();

		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL CText2D_s::SetText(int x,int y,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color)
	{
		if( pFont == NULL)
			return FALSE;

		if( m_String == str && m_pFont == pFont && m_Color == color)
			return FALSE;

		m_String = str;
		
		m_Rect = sRect( sPoint( x, y), sSize( 0, 0));

		if( m_String.size() == 0)
		{
			return FALSE;
		}
		
		m_pFont = pFont;

		SelectObject( g_NullDC, pFont->m_hFont);

		sSize text_size;
			
		GetTextExtentPoint32( g_NullDC, str, _tcslen( str), &text_size);

		m_Rect.right = m_Rect.left + text_size.cx;
		m_Rect.bottom = m_Rect.bottom + text_size.cy;

		if( !CreateTexture( text_size))
			return FALSE;

		// 문자하나하나를 읽어서 해줌
		int i,j;
		int tx = 0;
		TCHAR *pCur = (TCHAR *)str;

		GLYPHMETRICS gm;
		MAT2		 mat2;

		ZeroMemory( &mat2, sizeof( MAT2));
		mat2.eM11.value = 1;
		mat2.eM22.value = 1;

		m_OutlineColor = (unsigned short)0x8000 | (outline_color << 10) | (outline_color) << 5 | outline_color;

		// 임시 저장소
		unsigned short *pTextTempBuffer = g_TextTempBuffer;

		int nCount = m_TextureList.size();
		for(i = 0; i < nCount; ++i)
		{
			sTexture *pTempTexture = &m_TextureList[ i];
			int size = pTempTexture->m_TextureSize.cx * pTempTexture->m_TextureSize.cy;
			g_TextBufferTemp[ i] = pTextTempBuffer;

			ZeroMemory( g_TextBufferTemp[ i], size * 2);
			
			pTextTempBuffer += size;
		}

		sTexture *pTexture = &m_TextureList[ 0];
		int tex_index = 0;

		unsigned short *pTextureBuffer		 = g_TextBufferTemp[ 0];
		unsigned short *pTextureBuffer_Second = NULL;

		while( *pCur != '\0')
		{
			UINT nChar = 0;

			if( IsDBCSLeadByte( *pCur))
			{
				nChar = (BYTE)(*pCur++);
				nChar <<= 8;
				nChar |= (BYTE)(*pCur++);
			}
			else
				nChar = (BYTE)*pCur++;

			DWORD c_size = GetGlyphOutline( g_NullDC, nChar, GGO_BITMAP, &gm, 0, 0, &mat2);	

			c_size = GetGlyphOutline( g_NullDC, nChar, GGO_BITMAP, &gm, c_size, m_cBuffer, &mat2);

			int char_width;
			GetCharWidth( g_NullDC, nChar, nChar, &char_width);

			if( c_size > 0)
			{
				// 글자가 걸칠경우 그냥 두번 그려준다

				if( tx + char_width > pTexture->m_TextureSize.cx)
				{
					// 두개 락걸기
					if( tex_index + 1 > m_TextureList.size() - 1)
						break;

					sTexture *pSecond = &m_TextureList[ tex_index + 1];
					pTextureBuffer_Second = g_TextBufferTemp[ tex_index + 1];

					int byte_xsize = gm.gmBlackBoxX / 8;

					if( byte_xsize == 0)
						byte_xsize = 1;

					int align = ( 4 - (byte_xsize) % 4) % 4;
					int pitch = (byte_xsize)+ align;

					for(i = 0; i < gm.gmBlackBoxY; ++i)
					{
						for(j = 0; j < gm.gmBlackBoxX; ++j)
						{
							if( !GetMonochromBit( m_cBuffer + (gm.gmBlackBoxY - i - 1) * pitch, j))
								continue;

							int x_position = tx + gm.gmptGlyphOrigin.x + j;

							SHORT *pDest;
 
							if( x_position > pTexture->m_TextureSize.cx - 1)
							{

								pDest = (SHORT *)((BYTE *)pTextureBuffer_Second + 
											+ pSecond->m_TextureSize.cx * 2 * (m_pFont->m_TextMetric.tmAscent + gm.gmBlackBoxY - gm.gmptGlyphOrigin.y - i - 1)
											+ (x_position - (pTexture->m_TextureSize.cx - 1) - 1) * 2);
							}
							else
							{
								pDest = (SHORT *)((BYTE *)pTextureBuffer
											+ pTexture->m_TextureSize.cx * 2 * (m_pFont->m_TextMetric.tmAscent + gm.gmBlackBoxY - gm.gmptGlyphOrigin.y - i - 1)
											+ x_position * 2);

							}

							*pDest = (unsigned short)0xFFFF;
						}
					}

					pTexture->m_ImageSize.cx = pTexture->m_TextureSize.cx;
					tx -=  pTexture->m_TextureSize.cx - char_width;

					pTexture = pSecond;
					pTextureBuffer = pTextureBuffer_Second;

					pSecond->m_ImageSize.cx = tx;
					
					++tex_index;

				}
				else
				{
					int byte_xsize = gm.gmBlackBoxX / 8;

					if( byte_xsize == 0)
						byte_xsize = 1;

					int align = ( 4 - (byte_xsize) % 4) % 4;
					int pitch = (byte_xsize)+ align;

					for(i = 0; i < gm.gmBlackBoxY; ++i)
					{
						for(j = 0; j < gm.gmBlackBoxX; ++j)
						{
							if( !GetMonochromBit( m_cBuffer + (gm.gmBlackBoxY - i - 1) * pitch, j))
								continue;

							//int x_position = tx + gm.gmptGlyphOrigin.x + j;

							SHORT *pDest = (SHORT *)((BYTE *)pTextureBuffer
								+ pTexture->m_TextureSize.cx * 2 * (m_pFont->m_TextMetric.tmAscent + gm.gmBlackBoxY - gm.gmptGlyphOrigin.y - i - 1)
								+ (gm.gmptGlyphOrigin.x + tx + j) * 2);

							*pDest = (unsigned short)0xFFFF;
						}
					}
					tx += char_width;
				}
			}
			else
				tx += char_width;

		}

		for(i = 0; i < m_TextureList.size(); ++i)
		{
			sTexture *pTempTexture = &m_TextureList[ i];
			
#if 1
			// Filtering을 한번 해보겠다
			int index_x;
			int index_y;

			unsigned short *pFilterBuffer = g_TextBufferTemp[ i];
			unsigned short black = 0;

#define FILTER_PIXEL( x, y) (((x) < 0 || (y) < 0 || (x) > pTempTexture->m_TextureSize.cx - 1 || (y) > pTempTexture->m_TextureSize.cy) ? &black : (pFilterBuffer + (x) + (y) * pTempTexture->m_TextureSize.cx))

			for(index_x = 0; index_x < pTempTexture->m_TextureSize.cx; ++index_x)
			{
				for(index_y = 0; index_y < pTempTexture->m_TextureSize.cy; ++index_y)
				{
					unsigned short *pPixel[ 9];

					pPixel[ 0] = FILTER_PIXEL( index_x, index_y);
					
					if( *pPixel[ 0] == (unsigned short)0xFFFF)
					{					
						pPixel[ 1] = FILTER_PIXEL( index_x - 1, index_y - 1);
						pPixel[ 2] = FILTER_PIXEL( index_x, index_y - 1);
						pPixel[ 3] = FILTER_PIXEL( index_x + 1, index_y - 1);
						
						pPixel[ 4] = FILTER_PIXEL( index_x - 1, index_y);
						pPixel[ 5] = FILTER_PIXEL( index_x + 1, index_y);
						
						pPixel[ 6] = FILTER_PIXEL( index_x - 1, index_y + 1);
						pPixel[ 7] = FILTER_PIXEL( index_x, index_y + 1);
						pPixel[ 8] = FILTER_PIXEL( index_x + 1, index_y + 1);

						for(int m = 1; m < 9; ++m)
						{
							if( *pPixel[ m] != 0)
								continue;

							*pPixel[ m] = (unsigned short)m_OutlineColor;
						}
					}
				}
			}
		
#endif			

			D3DLOCKED_RECT d3d_rect;
			pTempTexture->m_pTexture->LockRect( 0, &d3d_rect, 0, 0);
			for(j = 0; j < pTempTexture->m_TextureSize.cy; ++j)
			{
				memcpy( (unsigned char *)d3d_rect.pBits + j * d3d_rect.Pitch, g_TextBufferTemp[ i] + j * pTempTexture->m_TextureSize.cx, pTempTexture->m_TextureSize.cx * 2);
			}
			pTempTexture->m_pTexture->UnlockRect( 0);
		}

		pTexture->m_ImageSize.cx = tx;

		m_Color = color;
		
		PrepareRender();

		return TRUE;
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL CText2D_s::CreateTexture(sSize size)
	{
		if( m_TextureList.size() > 0)
		{
			if( size.cx <= m_TextureSize.cx && size.cy <= m_TextureSize.cy)
				return TRUE;
		}

		ReleaseTexture();

		HRESULT hr;

		// Texture사이즈 구하기
		int xcount = size.cx / 256 + (((size.cx % 256) == 0) ? 0 : 1);
		int ycount = size.cy / 256 + (((size.cy % 256) == 0) ? 0 : 1);

		m_TextureSize = sSize( 0, 0);

		int i,j;

		for(i = 0; i < xcount; ++i)
		{
			for(j = 0; j < ycount; ++j)
			{
				UINT xsize = size.cx - i * 256;
				UINT ysize = size.cy - j * 256;
				UINT mipmap = 1;
				D3DFORMAT format = D3DFMT_A1R5G5B5;	

				if( xsize > 256)
					xsize = 256;

				if( ysize > 256)
					ysize = 256;

				sSize or_size( xsize, ysize);

				hr = D3DXCheckTextureRequirements( g_pDirect3DDevice, &xsize, &ysize, &mipmap,  0, &format, D3DPOOL_MANAGED);

				if( FAILED( hr))
				{
					ReleaseTexture();
					return FALSE;
				}

				IDirect3DTexture9* pTexture;

				hr = D3DXCreateTexture( g_pDirect3DDevice, xsize, ysize, mipmap,  0, format, D3DPOOL_MANAGED, &pTexture);
				
				if( FAILED( hr))
				{
					ReleaseTexture();
					return FALSE;
				}

				m_TextureSize.cx += xsize;
				m_TextureSize.cy += ysize;

				sTexture tex;

				tex.m_pTexture = pTexture;
				tex.m_Position = sPoint( i * 256, j * 256);
				tex.m_ImageSize = or_size;
				tex.m_TextureSize = sSize( xsize, ysize);

				m_TextureList.push_back( tex);

			}
		}

		return TRUE;
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API void CText2D_s::ReleaseTexture()
	{
		for(TEXTURELIST::iterator it = m_TextureList.begin(); it != m_TextureList.end(); ++it)
		{
			sTexture &tex = *it;

			tex.m_pTexture->Release();
		}

		m_TextureList.clear();
	}

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL CText2D_s::SetParentRect(sRect *pParentRect)
	{
		m_pParentRect = pParentRect;
	
		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL CText2D_s::Render(float fix_x, float fix_y)
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);


		if( m_pParentRect != NULL && m_nFace)
		{
			VT_TLVertex* pVertex;
			m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

			for(register int i = 0; i < m_nFace; ++i)
			{
				sTexture *pTexture = &m_TextureList[ i];

				sRect rect( pTexture->m_Position, pTexture->m_ImageSize );
				sPoint position( m_Rect.left + m_pParentRect->left, m_Rect.top + m_pParentRect->top);
				rect += position;

				pVertex[i*4+0].pos = Vector4( rect.left, rect.bottom, 0, 1);
				pVertex[i*4+1].pos = Vector4( rect.left, rect.top, 0, 1);
				pVertex[i*4+2].pos = Vector4( rect.right, rect.bottom, 0, 1);
				pVertex[i*4+3].pos = Vector4( rect.right, rect.top, 0, 1);
				for(register int j = 0; j < 4; ++j)
				{
					pVertex[i*4+j].pos.x = (int)pVertex[i*4+j].pos.x - 0.5f - fix_x;
					pVertex[i*4+j].pos.y = (int)pVertex[i*4+j].pos.y - 0.5f - fix_y;
				}
			}// for

			m_VB->Unlock();
		}// if

		g_Device.SetStreamSource( m_VB, sizeof(VT_TLVertex) );
		g_Device.SetFVF(D3DFVF_TLVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX);

		for(int i=0; i < m_nFace; ++i)
		{
			sTexture *pTexture = &m_TextureList[ i];

			assert(pTexture);

			g_Device.SetTexture(0, pTexture->m_pTexture);
			//g_pDirect3DDevice->SetTexture( 0, pTexture->m_pTexture);

			g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2);
		}// for

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);

		return TRUE;
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL CText2D_s::PrepareRender()
	{
		m_nFace = 0;

		//YS_TEMP
		if( m_String.size() == 0 || m_TextureSize.cx == 0 || !m_VB )
		{
			return TRUE;
		}

		int nCount = m_TextureList.size();

		if(nCount)
		{
			if(nCount == 1)
			{
				sTexture &tex = m_TextureList[0];

				sRect rect( tex.m_Position, tex.m_ImageSize);

				if( m_Rect.Width() > tex.m_Position.x)
				{
					rect.left	+= m_Rect.left;
					rect.right	+= m_Rect.left;
					rect.top	+= m_Rect.top;
					rect.bottom += m_Rect.top;

					float tx2 = (float)(tex.m_ImageSize.cx)/ (float)tex.m_TextureSize.cx;
					float ty2 = (float)(tex.m_ImageSize.cy)/ (float)tex.m_TextureSize.cy;

					VT_TLVertex Vertex[4];
					Vertex[0].pos = Vector4( rect.left, rect.bottom, 0, 1)		- Vector4( 0.5f, 0.5f, 0, 0);
					Vertex[1].pos = Vector4( rect.left, rect.top, 0, 1)			- Vector4( 0.5f, 0.5f, 0, 0);
					Vertex[2].pos = Vector4( rect.right, rect.bottom, 0, 1)		- Vector4( 0.5f, 0.5f, 0, 0);
					Vertex[3].pos = Vector4( rect.right, rect.top, 0, 1)		- Vector4( 0.5f, 0.5f, 0, 0);

					Vertex[0].tex = Vector2( 0,		ty2);
					Vertex[1].tex = Vector2( 0,		0);
					Vertex[2].tex = Vector2( tx2,	ty2);
					Vertex[3].tex = Vector2( tx2,	0);

					Vertex[0].diffuse = Vertex[1].diffuse = Vertex[2].diffuse = Vertex[3].diffuse = m_Color;

					
					VOID* pVertices = NULL;
					if( !FAILED( m_VB->Lock( 0, sizeof(Vertex), (void**)&pVertices, 0 )))
					{
						memcpy( pVertices, Vertex, sizeof(Vertex) );
						m_VB->Unlock();
					}

					m_nFace += 2;
				}				
			}
			else
			{
				VT_TLVertex* pVertex;
				m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

				for(int i = 0; i < nCount; ++i)
				{
					sTexture &tex = m_TextureList[ i];

					sRect rect( tex.m_Position, tex.m_ImageSize);

					if( m_Rect.Width() < tex.m_Position.x)
						break;

					rect.left	+= m_Rect.left;
					rect.right	+= m_Rect.left;
					rect.top	+= m_Rect.top;
					rect.bottom += m_Rect.top;

					float tx2 = (float)(tex.m_ImageSize.cx)/ (float)tex.m_TextureSize.cx;
					float ty2 = (float)(tex.m_ImageSize.cy)/ (float)tex.m_TextureSize.cy;

					pVertex[i*4 +  0].pos = Vector4( rect.left, rect.bottom, 0, 1)	- Vector4( 0.5f, 0.5f, 0, 0);
					pVertex[i*4 +  1].pos = Vector4( rect.left, rect.top, 0, 1)		- Vector4( 0.5f, 0.5f, 0, 0);
					pVertex[i*4 +  2].pos = Vector4( rect.right, rect.bottom, 0, 1)	- Vector4( 0.5f, 0.5f, 0, 0);
					pVertex[i*4 +  3].pos = Vector4( rect.right, rect.top, 0, 1)	- Vector4( 0.5f, 0.5f, 0, 0);

					pVertex[i*4 + 0].tex = Vector2( 0,	ty2);
					pVertex[i*4 + 1].tex = Vector2(	0,	0);
					pVertex[i*4 + 2].tex = Vector2( tx2, ty2);
					pVertex[i*4 + 3].tex = Vector2( tx2, 0);

					pVertex[i*4 + 0].diffuse = pVertex[i*4 + 1].diffuse = pVertex[i*4 + 2].diffuse = pVertex[i*4 + 3].diffuse = m_Color;

					m_nFace += 2;
				}

				m_VB->Unlock();
			}
		}		
		

		m_nFace /= 2;

		return TRUE;
	}

	XIAHGE_API void CText2D_s::SetColor(D3DCOLOR dwColor)
	{
		m_Color = dwColor;

		PrepareRender();
	}

};
