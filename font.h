#pragma once

//
//	D3D FONT CORE
//

namespace XiahGameEngine
{
	// ALIGN DEFINE

	#define TEXT2D_ALIGN_HLEFT		0x00000F
	#define TEXT2D_ALIGN_HRIGHT		0x0000F0
	#define TEXT2D_ALIGN_HCENTER	0x000F00
	#define TEXT2D_ALIGN_VTOP		0x00F000
	#define TEXT2D_ALIGN_VBOTTOM	0x0F0000
	#define TEXT2D_ALIGN_VCENTER	0xF00000
	#define TEXT2D_ALIGN_CENTER		(TEXT2D_ALIGN_VCENTER | TEXT2D_ALIGN_HCENTER)

	class CText2D_core
	{
	public :
		XIAHGE_API	void SetPosition(int x,int y);
		XIAHGE_API	void FixPositionByRect(int x,int y);

		XIAHGE_API	BOOL CreateTexture(char *ch1, sFont	*pFont);
		XIAHGE_API	BOOL CreateVertex();
		XIAHGE_API	BOOL Render();

		XIAHGE_API	unsigned short Make32To16(unsigned long RGB);

		XIAHGE_API	BOOL SetText(int x,int y,char *ch, sFont *pFont, bool mbcs,D3DCOLOR color,unsigned char outline_color);
		XIAHGE_API	void MakeOutLine(unsigned char *buffer);
		XIAHGE_API	int GetXSize();
		XIAHGE_API	int GetYSize();

		XIAHGE_API	CText2D_core();
		XIAHGE_API	~CText2D_core();

	protected :
		unsigned char		m_con[3];
		sSize				m_TextureSize;
		sPoint				m_Position;
		bool				m_IsMBCS;		// Is Multi Byte Character Set ?

		LPDIRECT3DVERTEXBUFFER9	m_VB;
		IDirect3DTexture9*	m_pTexture;
		D3DCOLOR			m_Color;
	};

	//////////////////////////////////////////////////////////////////////////

	#define	MAX_MULTILINES		32
	#define	LINESKIP			3
	#define SEPARATE_MARK		'|'

	class CText2D : public std::vector<CText2D_core *>
	{
	public :
		XIAHGE_API CText2D();
		XIAHGE_API ~CText2D();

		XIAHGE_API BOOL SetText(int x,int y,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color = 0, int nColorCount = 0, ...);
		XIAHGE_API BOOL SetText(sRect *pRect,DWORD Align,LPCTSTR str,sFont *pFont,D3DCOLOR color,unsigned char outline_color = 0);
		XIAHGE_API BOOL Render(float fix_x = 0.0f,float fix_y = 0.0f);

		XIAHGE_API BOOL Release();
		XIAHGE_API BOOL SetParentRect(sRect *pParentRect);
		XIAHGE_API BOOL	Clear();

		XIAHGE_API sSize GetSize();
		XIAHGE_API sSize GetSize(int nNum);

		XIAHGE_API int Calc_Lines(LPCTSTR str);
		XIAHGE_API char* Get_Lines(LPCTSTR str,int line);

	protected:
		XIAHGE_API sSize PreCalcStrXsize(char *str, sFont *pFont);
		XIAHGE_API sSize CalcStrXsize(char *str, sFont *pFont);

		XIAHGE_API void Clear_Val();
		XIAHGE_API BOOL Add_Text(int x,int y,char *str,sFont *pFont,D3DCOLOR color,unsigned char outline_color,bool multybyte);
		XIAHGE_API BOOL DeleteAll_Text();

		XIAHGE_API int GetAlignX(int Width,int Text_width,DWORD Align);
		XIAHGE_API int GetAlignY(int Height,int Text_height,DWORD Align);

		sRect*	m_pParentRect;

		int		m_pre_xsize;
		int		m_pre_ysize;

		sSize	m_LineSize[MAX_MULTILINES];

		char	m_precontext[1024];
	};

};