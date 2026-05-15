//
//	D3D Font Core
//	한글자당으로 수정한다. 
//	

#include "stdafx.h"
#include "Text2d.h"

namespace XiahGameEngine
{

	XIAHGE_API	CText2D_core::CText2D_core()
	{
		m_IsMBCS = false;

		m_VB  = NULL;
		m_pTexture = NULL;

		// default color
		m_Color = D3DCOLOR(RGB(0,0,255));

		m_con[0] = 0;
		m_con[1] = 0;
		m_con[2] = 0;
	}

	XIAHGE_API	CText2D_core::~CText2D_core()
	{
		// Vertex Buffer Release
		if(m_VB)
			m_VB->Release();
		
		// Texture Release
		if(m_pTexture)
			m_pTexture->Release();
	}

	//////////////////////////////////////////////////////////////////////////

	XIAHGE_API	BOOL CText2D_core::SetText(int x,int y,char *ch, sFont *pFont, bool mbcs,D3DCOLOR color,unsigned char outline_color)
	{
		m_IsMBCS = mbcs;

		if(m_IsMBCS)
		{
			if(m_con[0] == *ch && m_con[1] == *(ch+1)) return FALSE;
		}
		else
			if(m_con[0] == *ch) return FALSE;

		// Texture 생성
		if(CreateTexture(ch,pFont) == FALSE) return FALSE;

		m_Color = color;

		// Vertex 생성
		CreateVertex();

		if(m_IsMBCS)
		{
			m_con[0] = *ch;
			m_con[1] = *(ch+1);
		}
		else
			m_con[0] = *ch;


		SetPosition(x,y);

		return TRUE;
	}

	// Texture 생성 (m_TextureSize크기의 Texture)
	XIAHGE_API	BOOL CText2D_core::CreateVertex()
	{
		float tx1, ty1, tx2, ty2;

		tx1 = 0;
		ty1 = 0;
		tx2 = 1.0f;
		ty2 = 1.0f;

		sRect	rect;
		rect.left = m_Position.x;
		rect.right = m_Position.x + m_TextureSize.cx;
		rect.top = m_Position.y;
		rect.bottom = m_Position.y + m_TextureSize.cy;

		g_pDirect3DDevice->CreateVertexBuffer( 4 * sizeof(VT_TLVertex),D3DUSAGE_WRITEONLY ,D3DFVF_TLVERTEX, D3DPOOL_MANAGED, &m_VB, NULL);

		VT_TLVertex* pVertex;
		m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

		pVertex[0].pos = Vector4( rect.left, rect.bottom, 0, 1)	- Vector4( 0.5f, 0.5f, 0, 0);
		pVertex[1].pos = Vector4( rect.left, rect.top, 0, 1)	- Vector4( 0.5f, 0.5f, 0, 0);
		pVertex[2].pos = Vector4( rect.right, rect.bottom, 0, 1)- Vector4( 0.5f, 0.5f, 0, 0);
		pVertex[3].pos = Vector4( rect.right, rect.top, 0, 1)	- Vector4( 0.5f, 0.5f, 0, 0);

		pVertex[0].tex = Vector2( tx1, ty2);
		pVertex[1].tex = Vector2( tx1, ty1);
		pVertex[2].tex = Vector2( tx2, ty2);
		pVertex[3].tex = Vector2( tx2, ty1);

		pVertex[0].diffuse = m_Color;
		pVertex[1].diffuse = m_Color;
		pVertex[2].diffuse = m_Color;
		pVertex[3].diffuse = m_Color;

		m_VB->Unlock();
		return TRUE;
	}


	XIAHGE_API	int CText2D_core::GetXSize()
	{
		return m_TextureSize.cx;
	}

	XIAHGE_API	int CText2D_core::GetYSize()
	{
		return m_TextureSize.cy;
	}

	#define MAX_CHARACTER_SIZE	64

	XIAHGE_API	unsigned short CText2D_core::Make32To16(unsigned long RGB)
	{
		unsigned short ret;
		ret = (unsigned short)( ((RGB >> 16) & 0x1f) << 11 | ((RGB >> 8) & 0x3f) << 5 | (RGB & 0x1f) );
		return ret;
	}

	// Buffer에 Outline을 만든다
	XIAHGE_API	void CText2D_core::MakeOutLine(unsigned char *buffer)
	{
		int index_x;
		int index_y;

		unsigned short m_OutlineColor = (unsigned short)0x8000| (0x0a << 10) | (0x0a ) << 5 | 0x0a ;
		unsigned short *pFilterBuffer = (unsigned short*)buffer;
		unsigned short black = 0;

		#define FILTER_PIXEL( x, y) (((x) < 0 || (y) < 0 || (x) > m_TextureSize.cx - 1 || (y) > m_TextureSize.cy) \
									? &black : (pFilterBuffer + (x) + (y) * m_TextureSize.cx))

		for(index_x = 0; index_x < m_TextureSize.cx; ++index_x)
		{
			for(index_y = 0; index_y < m_TextureSize.cy; ++index_y)
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
	}


	// Texture 생성 (m_TextureSize크기의 Texture)
	XIAHGE_API	BOOL CText2D_core::CreateTexture(char *ch1, sFont *pFont)
	{
		HRESULT hr;
		HDC	dc;
		HBITMAP bitmap;
		LOGFONT lf;
		//HFONT	font;

		//////////////////////////////////////////////////////////////////////////
		memset(&lf,0,sizeof(lf));

		lf.lfCharSet = HANGUL_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy(lf.lfFaceName,pFont->m_strFontName);
		lf.lfHeight = pFont->m_nFontHeight;
		lf.lfWidth = 0;
		lf.lfWeight = FW_NORMAL;

		//////////////////////////////////////////////////////////////////////////
		
		HDC hdc = GetDC(g_EngineInfo.m_hWnd);
		dc = CreateCompatibleDC(hdc);

		// max size크기의 DC 만들고
		bitmap = CreateCompatibleBitmap(hdc, MAX_CHARACTER_SIZE, MAX_CHARACTER_SIZE);
		pFont->m_hFont = CreateFontIndirect(&lf);

		// Font  지정
		SelectObject(dc, bitmap);
		HFONT fontbk = (HFONT) SelectObject(dc, pFont->m_hFont);		// backup font ?

		SetBkColor(dc, 0x000000);	// 검정
		SetTextColor(dc, 0xFFFFFF);	// 흰색

		// texture
		unsigned short *img;

		// single byte이면 1개 double byte이면 2개를 한꺼분에 찍는다
		GetTextExtentPoint32( dc, ch1, m_IsMBCS == false ?  1:2 , &m_TextureSize);
		TextOut(dc, 0, 0, ch1, m_IsMBCS == false ? 1:2 );

		// overflow ERROR (글씨가 너무 크다)
		if(m_TextureSize.cx >= MAX_CHARACTER_SIZE || m_TextureSize.cy >= MAX_CHARACTER_SIZE || m_TextureSize.cx <= 0 || m_TextureSize.cy <= 0)
			return FALSE;

		// 16 bit texture
		img = new unsigned short [m_TextureSize.cx * m_TextureSize.cy];

		int x, y;
		int c,j;

		// DC에서 texture로 이미지를 가져온다 (4Byte)
		for(y=0, j=0; y < m_TextureSize.cy; y++)
		{
			for(x=0; x<m_TextureSize.cx; x++)
			{
				c = GetPixel(dc, x, y);

				if(c == 0xffffff)
					img[j++] = 0xffff;
				else
					img[j++] = 0x0;
			}
		}

		MakeOutLine((unsigned char *)img);

		// Texture를 enum 하는것은 일단 미룬다
		D3DFORMAT format = D3DFMT_A1R5G5B5;
		hr = D3DXCreateTexture( g_pDirect3DDevice, m_TextureSize.cx, m_TextureSize.cy, 1,  0, format, D3DPOOL_MANAGED, &m_pTexture);
		if( FAILED( hr)) return FALSE;

		D3DLOCKED_RECT d3d_rect;
		unsigned char *tag,*stag;
		int pitch;

		m_pTexture->LockRect( 0, &d3d_rect, 0, 0);
		tag = (unsigned char *)d3d_rect.pBits;
		stag = (unsigned char *)img;
		pitch = d3d_rect.Pitch;

		for(int j = 0; j < m_TextureSize.cy;  j++)
		{
			memcpy(tag + j * pitch, (unsigned char*)(stag + j * m_TextureSize.cx * 2), m_TextureSize.cx * 2);
		}

		m_pTexture->UnlockRect( 0);
		
		delete [] img;
		DeleteObject(pFont->m_hFont);
		DeleteObject(bitmap);
		DeleteDC(dc);
		ReleaseDC(g_EngineInfo.m_hWnd, hdc);

		return TRUE;
	}

	XIAHGE_API	void CText2D_core::SetPosition(int x,int y)
	{
		sRect	rect;

		m_Position.x = x;
		m_Position.y = y;

		rect.left = m_Position.x;
		rect.right = (m_Position.x) + m_TextureSize.cx;
		rect.top = (m_Position.y);
		rect.bottom = (m_Position.y) + m_TextureSize.cy;

		VT_TLVertex* pVertex;
		m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

		pVertex[0].pos = Vector4( rect.left - 0.5f, rect.bottom - 0.5f, 0, 1);
		pVertex[1].pos = Vector4( rect.left - 0.5f, rect.top - 0.5f, 0, 1);
		pVertex[2].pos = Vector4( rect.right- 0.5f, rect.bottom - 0.5f, 0, 1);
		pVertex[3].pos = Vector4( rect.right- 0.5f, rect.top - 0.5f, 0, 1);

		m_VB->Unlock();
	}

	XIAHGE_API	void CText2D_core::FixPositionByRect(int x,int y)
	{
		sRect	rect;

		rect.left = m_Position.x + x;
		rect.right = (m_Position.x + x) + m_TextureSize.cx;
		rect.top = (m_Position.y + y);
		rect.bottom = (m_Position.y + y) + m_TextureSize.cy;

		VT_TLVertex* pVertex;
		m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

		pVertex[0].pos = Vector4( rect.left - 0.5f, rect.bottom - 0.5f, 0, 1);
		pVertex[1].pos = Vector4( rect.left - 0.5f, rect.top - 0.5f, 0, 1);
		pVertex[2].pos = Vector4( rect.right- 0.5f, rect.bottom - 0.5f, 0, 1);
		pVertex[3].pos = Vector4( rect.right- 0.5f, rect.top - 0.5f, 0, 1);

		m_VB->Unlock();
	}

	XIAHGE_API	BOOL CText2D_core::Render()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);

		g_pDirect3DDevice->SetStreamSource( 0, m_VB, 0 , sizeof(VT_TLVertex) );
		g_pDirect3DDevice->SetTexture( 0, m_pTexture);
		g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX);
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

		return TRUE;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	XIAHGE_API CText2D::CText2D()
	{
		m_pParentRect = NULL;
		memset(m_precontext,0,sizeof(m_precontext));

		Clear_Val();
	}

	XIAHGE_API CText2D::~CText2D()
	{
		DeleteAll_Text();
	}

	XIAHGE_API void CText2D::Clear_Val()
	{
		m_pre_xsize = 0;
		m_pre_ysize = 0;

		for(int i=0; i<MAX_MULTILINES;i++)
		{
			m_LineSize[i].cx = 0;
			m_LineSize[i].cy = 0;
		}
	}


	XIAHGE_API sSize CText2D::PreCalcStrXsize(char *str, sFont *pFont)
	{
		char temp[1024] = {0,};
		int len = strlen(str);

		for(int i =0; i<len; i++)
		{
			char c = *(str+i);
			if(c == SEPARATE_MARK)
				break;
			temp[i] = c;
		}

		return CalcStrXsize(temp,pFont);
	}

	XIAHGE_API sSize CText2D::CalcStrXsize(char *str, sFont *pFont)
	{
		HDC	dc;
		HBITMAP bitmap;
		LOGFONT lf;
		sSize	tempSize;

		memset(&lf,0,sizeof(lf));
		lf.lfCharSet = HANGUL_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy(lf.lfFaceName,pFont->m_strFontName);
		lf.lfHeight = pFont->m_nFontHeight;
		lf.lfWidth = 0;
		lf.lfWeight = FW_NORMAL;
/*
		HDC hdc = GetDC(g_EngineInfo.m_hWnd);
		dc = CreateCompatibleDC(hdc);

		// 1024 * 32 Bitmap에 찍는다
		bitmap = CreateCompatibleBitmap(hdc, 1024, 32);

		SelectObject(dc, bitmap);
		HFONT fontbk = (HFONT) SelectObject(dc, pFont->m_hFont);
		//int len = strlen(str);
		GetTextExtentPoint32( dc, str, _tcslen(str) , &tempSize);


		DeleteObject(pFont->m_hFont);
		DeleteObject(bitmap);
		DeleteDC(dc);
		ReleaseDC(g_EngineInfo.m_hWnd, hdc);

*/
		SelectObject( g_NullDC, pFont->m_hFont);
		GetTextExtentPoint32( g_NullDC, str, _tcslen(str) , &tempSize);

		tempSize.cx++;
		tempSize.cy++;
		return tempSize;
	}


	XIAHGE_API int CText2D::GetAlignX(int Width,int Text_width,DWORD Align)
	{
		int x_pos = 0;

		switch( Align & 0x000FFF)
		{
		case TEXT2D_ALIGN_HLEFT:
			x_pos = 0;
			break;
		case TEXT2D_ALIGN_HRIGHT:
			x_pos = Width - Text_width;
			break;
		case TEXT2D_ALIGN_HCENTER:
			x_pos = (Width - Text_width) / 2;
			break;
		}
		return x_pos;
	}

	XIAHGE_API int CText2D::GetAlignY(int Height,int Text_height,DWORD Align)
	{
		int y_pos = 0;

		switch( Align & 0xFFF000)
		{
		case TEXT2D_ALIGN_VTOP:
			y_pos = 0;
			break;
		case TEXT2D_ALIGN_VBOTTOM:
			y_pos = Height- Text_height;
			break;
		case TEXT2D_ALIGN_VCENTER:
			y_pos = (Height- Text_height) / 2;
			break;
		}

		return y_pos;
	}

	// Line당 컬러를 다르게 지정
	XIAHGE_API BOOL CText2D::SetText(int x,int y,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color, int nColorCount, ...)
	{
		int line = 0;
		int posx,posy;
		int lines = 0;
		int len = 0;
		char *m_str;
		sSize	size;
		int lineskip = 0;

		int total_len = _tcslen(str);
		if(NULL == pFont || total_len < 1 || strcmp(m_precontext,str) == 0) return FALSE;

		DeleteAll_Text();
		strcpy(m_precontext,str);

		posx = x;
		posy = y;

		va_list ap;
		if(nColorCount > 0) va_start( ap, nColorCount);

		lines = Calc_Lines(str);
		for(int m_line=0; m_line<lines;m_line++)
		{
			// 해당 라인을 얻는다.
			m_str = Get_Lines(str,m_line);
			len = _tcslen(m_str);

			size = PreCalcStrXsize(m_str,pFont);
			m_LineSize[line++] = size;

/*
			// check line skip
			if(*(str+i) == SEPARATE_MARK)
			{
				posx = x;
				posy += m_pre_ysize + LINESKIP;

				// 새로운 라인의 컬러를 얻는다 (있을경우에만)
				if(nColorCount > 0)
					color = va_arg( ap, D3DCOLOR);
				continue;
			}
*/
			for(int i=0; i<len;i++)
			{
				if(IsDBCSLeadByte(*(str+i)))
				{
					// multi character
					Add_Text(posx,posy + lineskip,(char*)m_str+i,pFont,color,outline_color,true);
					posx += m_pre_xsize;
					i++;
				}
				else
				{
					// single character
					Add_Text(posx,posy + lineskip,(char*)m_str+i,pFont,color,outline_color,false);
					posx += m_pre_xsize;
				}
			}

			if(nColorCount > 0)
				color = va_arg( ap, D3DCOLOR);
			lineskip += size.cy + LINESKIP;
		}

		if(nColorCount > 0)	va_end(ap);
		return TRUE;
	}

	// 해당라인의 string을 얻는다
	XIAHGE_API char* CText2D::Get_Lines(LPCTSTR str,int line)
	{
		static char buf[256];
		char ch;
		int c = 0;
		int howmanylines = 0;

		memset(buf,0,sizeof(buf));

		int len = _tcslen(str);
		for(int i=0;i<len;i++)
		{
			if(str[i] == SEPARATE_MARK)
			{
				howmanylines++;
			}
			else
			{
				ch = (char)str[i];
				if(ch == 0 || howmanylines > line) break;
				if(howmanylines == line)buf[c++] = ch;
			}
		}

		return buf;
	}


	// Line을 센다
	XIAHGE_API int CText2D::Calc_Lines(LPCTSTR str)
	{
		TCHAR buf[256] = {0,};
		int c = 0;
		int howmanylines = 0;

		int len = _tcslen(str);
		howmanylines = 1;
		for(int i=0;i<len;i++)
		{
			if(str[i] == SEPARATE_MARK)
			{
				if(c > 0 && i != len-1)
					howmanylines++;
			}
			else
			{
				buf[c++] = (TCHAR)str[i];
			}
		}
		return howmanylines;
	}

	// Default
	//
	XIAHGE_API BOOL CText2D::SetText(sRect *pRect,DWORD Align,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color)
	{
		int line = 0;
		int posx = 0;
		int posy = 0;
		int lines = 0;
		int len = 0;
		int lineskip = 0;

		sSize	size;
		char *m_str;

		int total_len = _tcslen(str);
		if(NULL == pFont || total_len < 1 || strcmp(m_precontext,str) == 0) return FALSE;

		DeleteAll_Text();
		// 보관 본 복사
		strcpy(m_precontext,str);

		lines = Calc_Lines(str);

		for(int m_line = 0; m_line < lines; m_line++)
		{
			// 해당 라인을 얻는다.
			m_str = Get_Lines(str,m_line);
			len = _tcslen(m_str);

			// 문자열의 X size를 얻구
			size = PreCalcStrXsize(m_str,pFont);
			m_LineSize[line++] = size;

			// Align에 대한 위치를 얻는다.
			posx = GetAlignX(pRect->Width(),size.cx,Align);
			posy = GetAlignY(pRect->Height(),size.cy,Align);

			for(int i=0; i<len;i++)
			{
				if(IsDBCSLeadByte(*(m_str+i)))
				{
					// multi character
					Add_Text(posx,posy + lineskip,(char*)m_str+i,pFont,color,outline_color,true);
					posx += m_pre_xsize;
					i++;
				}
				else
				{
					// single character
					Add_Text(posx,posy + lineskip,(char*)m_str+i,pFont,color,outline_color,false);
					posx += m_pre_xsize;
				}
			}

			//lineskip += m_pre_ysize + LINESKIP;
			lineskip += size.cy + LINESKIP;
		}

		return TRUE;
	}

	
	XIAHGE_API BOOL CText2D::Render(float fix_x,float fix_y)
	{
		iterator it;
		for(it = begin(); it != end(); it++)
		{
			CText2D_core *core = *it;

			if(m_pParentRect != NULL)
			{
				core->FixPositionByRect(m_pParentRect->left + fix_x,m_pParentRect->top + fix_y);
				//core->SetPosition(m_pParentRect->left,m_pParentRect->top);
			}

			core->Render();
		}
		return TRUE;
	}


	XIAHGE_API BOOL CText2D::Release()
	{
		DeleteAll_Text();
		return TRUE;
	}


	XIAHGE_API BOOL CText2D::SetParentRect(sRect *pParentRect)
	{
		m_pParentRect = pParentRect;
		return TRUE;
	}

	// 문자 추가
	XIAHGE_API BOOL CText2D::Add_Text(int x,int y,char *str,sFont *pFont,D3DCOLOR color,unsigned char outline_color,bool multybyte)
	{
		CText2D_core *new_text;
		new_text = new CText2D_core;
		if(NULL == new_text) return FALSE;

		new_text->SetText(x,y,str,pFont,multybyte,color,outline_color);
		push_back(new_text);

		// 방금전에 만든 글자의 크기를 보관
		m_pre_xsize = new_text->GetXSize();
		m_pre_ysize = new_text->GetYSize();

		return TRUE;
	}


	XIAHGE_API BOOL CText2D::DeleteAll_Text()
	{
		iterator it;

		Clear_Val();

		if(empty() == true) return FALSE;

		for(it = begin(); it != end(); it++)
		{
			CText2D_core *core = *it;
			delete core;
		}

		clear();

		memset(m_precontext,0,sizeof(m_precontext));
		return TRUE;
	}

	XIAHGE_API BOOL	CText2D::Clear()
	{
		DeleteAll_Text();
		return TRUE;
	}

	XIAHGE_API sSize CText2D::GetSize()
	{
		return m_LineSize[0];
	}

	XIAHGE_API sSize CText2D::GetSize(int nNum)
	{
		return m_LineSize[nNum];
	}

}