
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
								
	#define SMALL_FONT			GetFont( _T("ËÎÌå"), 11)
	#define DEFAULT_FONT		GetFont( _T("ËÎÌå"), 12)
	#define ITEMNAME_FONT		GetFont( _T("ËÎÌå"), 14)

	#define SMALL_FONT2			GetFont( _T("ËÎÌå"), 11)
	#define DEFAULT_FONT2		GetFont( _T("ËÎÌå"), 12)
	#define ITEMNAME_FONT2		GetFont( _T("ËÎÌå"), 14)

	#define DEFAULT_FONT_NAME		_T("ËÎÌå")
	#define DEFAULT_FONT_NAME_2		_T("ËÎÌå")

#else
	#define SMALL_FONT			GetFont( _T("SimSun"), 11)
	#define DEFAULT_FONT		GetFont( _T("SimSun"), 12)
	#define ITEMNAME_FONT		GetFont( _T("SimSun"), 14)

	#define SMALL_FONT2			GetFont( _T("SimSun"), 11)
	#define DEFAULT_FONT2		GetFont( _T("SimSun"), 12)
	#define ITEMNAME_FONT2		GetFont( _T("SimSun"), 14)

	#define DEFAULT_FONT_NAME		_T("SimSun")
	#define DEFAULT_FONT_NAME_2		_T("SimSun")
#endif

// color
#define YELLOW_COLOR		D3DCOLOR_XRGB( 255, 255, 0)
#define RED_COLOR			D3DCOLOR_XRGB( 255, 0, 0)
#define DEFAULT_COLOR		D3DCOLOR_XRGB( 255, 255, 255)

// align
#define DEFAULT_ALIGN		TEXT2D_ALIGN_CENTER
