
#define CTRL_BAR			0
#define CTRL_BUTTON			1
#define CTRL_COMBOBOX		2
#define CTRL_EDITBOX		3
#define CTRL_LISTBOX		4
#define CTRL_SCROLLBAR		5
#define CTRL_STATIC			6
#define CTRL_TOOLTIP		7
#define CTRL_FRAME			8
#define CTRL_STATICDUMMY	9

// size
#ifdef _CHINA_
								
	#define SMALL_FONT			GetFont( _T("ËÎÌו"), 11)
	#define DEFAULT_FONT		GetFont( _T("ËÎÌו"), 12)
	#define ITEMNAME_FONT		GetFont( _T("ËÎÌו"), 14)

	#define SMALL_FONT2			GetFont( _T("ËÎÌו"), 11)
	#define DEFAULT_FONT2		GetFont( _T("ËÎÌו"), 12)
	#define ITEMNAME_FONT2		GetFont( _T("ËÎÌו"), 14)

	#define DEFAULT_FONT_NAME		_T("ËÎÌו")
	#define DEFAULT_FONT_NAME_2		_T("ËÎÌו")

#else
	#define SMALL_FONT			GetFont( _T("±¼¸²Ã¼"), 11)
	#define DEFAULT_FONT		GetFont( _T("±¼¸²Ã¼"), 12)
	#define ITEMNAME_FONT		GetFont( _T("±¼¸²Ã¼"), 14)

	#define SMALL_FONT2			GetFont( _T("±¼¸²"), 11)
	#define DEFAULT_FONT2		GetFont( _T("±¼¸²"), 12)
	#define ITEMNAME_FONT2		GetFont( _T("±¼¸²"), 14)

	#define DEFAULT_FONT_NAME		_T("±¼¸²")
	#define DEFAULT_FONT_NAME_2		_T("±¼¸²Ã¼")
#endif

// color
#define YELLOW_COLOR		D3DCOLOR_XRGB( 255, 255, 0)
#define RED_COLOR			D3DCOLOR_XRGB( 255, 0, 0)
#define DEFAULT_COLOR		D3DCOLOR_XRGB( 255, 255, 255)

// align
#define DEFAULT_ALIGN		TEXT2D_ALIGN_CENTER
