#pragma once

namespace XiahGameEngine
{

#define TEXT2D_ALIGN_HLEFT		0x00000F
#define TEXT2D_ALIGN_HRIGHT		0x0000F0
#define TEXT2D_ALIGN_HCENTER	0x000F00
#define TEXT2D_ALIGN_VTOP		0x00F000
#define TEXT2D_ALIGN_VBOTTOM	0x0F0000
#define TEXT2D_ALIGN_VCENTER	0xF00000
#define TEXT2D_ALIGN_CENTER		(TEXT2D_ALIGN_VCENTER | TEXT2D_ALIGN_HCENTER)

	class CText2D_s : public CRenderObject
	{
	public:
		struct sTexture
		{
			IDirect3DTexture9* m_pTexture;
			sSize			   m_TextureSize;
			sSize			   m_ImageSize;
			sPoint			   m_Position;
		};

		typedef std::vector<sTexture> TEXTURELIST;

		DECLARE_RENDERTYPE(eRT_AlphaNoSort)
	public:
		XIAHGE_API CText2D_s();
		XIAHGE_API virtual ~CText2D_s();
		XIAHGE_API BOOL Create();
		XIAHGE_API BOOL Release();
	
		XIAHGE_API BOOL IsEmpty() { return m_String.size() == 0;}
		XIAHGE_API void Empty() { m_String.clear();}

		XIAHGE_API BOOL SetText(int x,int y,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color = 0);
		XIAHGE_API BOOL SetText(sRect *pRect,DWORD nAlign,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color = 0);
		XIAHGE_API BOOL SetParentRect(sRect *pParentRect);
		// [5/18/2005] 확장
		XIAHGE_API void SetColor(D3DCOLOR dwColor);

		XIAHGE_API virtual BOOL Render(float fix_x = 0.0f,float fix_y = 0.0f);
		XIAHGE_API BOOL PrepareRender();

		XIAHGE_API sSize GetSize(){ return sSize( m_Rect.Width(), m_Rect.Height());}

		sRect	m_Rect;
		TEXTURELIST		m_TextureList;

	protected:

		sString m_String;
		sRect*	m_pParentRect;
		sSize	m_TextureSize;
		sFont*	m_pFont;

		D3DCOLOR m_Color;
		unsigned short m_OutlineColor;

		//VT_TLVertex		m_Vertex[ 4 * 4];	// TextureSize는 256이니까 1024 / 256
		LPDIRECT3DVERTEXBUFFER9	m_VB;
		int				m_nFace;

		static BYTE  m_cBuffer[ 1024];

	protected:
		XIAHGE_API BOOL CreateTexture(sSize size);
		XIAHGE_API void ReleaseTexture();
	};

	// MULTILINE 기능추가(codesafe)
	// (기존의 func을 수정하지 않기 위하여 Class이름을 바꾸고 기존 Text2D class를 객체로 가지고 있는다.)
	// 멀티라인은 특정 char (| -> or)로 분리한다.
	// 예) 1우하하|2우캬캬|3우쿠쿠 -> 3줄로 표시된다.

	// 최대지원 멀티라인의 갯수
	#define MAX_MULTILINES	22 //HO_0410_07 상서령, 퀵 가이드 업데이트 : 최대 라인수 16에서 22로 변경
	// 분리 케렉터
	#define SEPARATE_MARK	'|'

	class CText2D
	{
	public:

		XIAHGE_API CText2D();
		XIAHGE_API ~CText2D();

		XIAHGE_API void Set_SkipLine(int skip = 3);	// 각라인당의 공백을 지정한다.
		XIAHGE_API BOOL SetText(int x,int y,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color = 0, int nColorCount = 0, ...);
		XIAHGE_API BOOL SetText(sRect *pRect,DWORD nAlign,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color = 0);

		// [5/18/2005] 확장
		XIAHGE_API void SetColor(D3DCOLOR dwColor);
		
		XIAHGE_API BOOL SetAlign(DWORD *align,int num);
		XIAHGE_API DWORD GetAlign();
		XIAHGE_API DWORD GetLineNum();

		XIAHGE_API BOOL Render(float fix_x = 0.0f,float fix_y = 0.0f);

		XIAHGE_API BOOL Release();
		XIAHGE_API BOOL SetParentRect(sRect *pParentRect);

		XIAHGE_API sSize GetSize()
		{
			return sSize(m_text2d[0].m_Rect.Width(),m_text2d[0].m_Rect.Height());
		}

		XIAHGE_API sSize GetSize(int nNum)
		{
			if(nNum >= MAX_MULTILINES)
				return sSize(0,0);

			return sSize(m_text2d[nNum].m_Rect.Width(),m_text2d[nNum].m_Rect.Height());
		}

		XIAHGE_API BOOL	Clear();

	protected:
		XIAHGE_API void Calc_Lines(LPCTSTR str);

	protected:
		CText2D_s	*m_text2d;	// 2D text Object

		DWORD m_Align[MAX_MULTILINES];
		TCHAR m_message[MAX_MULTILINES][64];
		int m_howmanylines;		// 총 몇라인 있어?
		int m_skip;
	};

};
