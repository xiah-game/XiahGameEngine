#pragma once
#include "cpu\\cpu.h"
#include "cpu\\optimize.h"

//HT_CHEAT : WINDOWSIZE
#define  G_WIDTH		1024
#define  G_HEIGHT		 768

namespace XiahGameEngine
{
	//HT_CHEAT : WINDOWSIZE
	XIAHGE_API extern RECT WindowRect;
	//-----------------------------------------------------------------------------------------------------------
	// 몸땅 inline으로 짜주마, 보기 복잡하면 나중에 분리해줘야지.
	struct sString : public std::basic_string<TCHAR>
	{
	public:
		inline sString()
		{
		}

		inline sString(const sString &a)
		{
			operator = (a);
		}

		inline sString(const TCHAR *str)
			: std::basic_string<TCHAR>(str)
		{
		
		}

		inline void printf(const TCHAR *format,...)
		{
			TCHAR buff[ 255];

			va_list ap;

			va_start( ap, format);
			_vstprintf( buff, format, ap);
			va_end( ap);

			std::basic_string<TCHAR>::operator =( buff);
		}

		inline operator LPCTSTR()
		{
			return (LPCTSTR)data();
		}

		inline void operator = (const std::basic_string<TCHAR> &a)
		{
			std::basic_string<TCHAR>::operator =( a);
		}

		inline void operator = (const TCHAR *str)	// 젠장 모호하긴 머가 모호해?
		{
			std::basic_string<TCHAR>::operator =(str);
		}

		inline void operator = (const sString &a)	// 미치겠네 인라인이라 자꾸이러나?
		{
			std::basic_string<TCHAR>::operator =(a);
		}

	// 편리한 넘들
	public:
		inline sString GetFilenameExceptExt()
		{
			sString r;
			
			int index = (int)find_last_of('.');

			if( index != -1)
				r = substr( 0, index + 1);
		
			return r;
		}

		inline sString GetPath()
		{
			sString r;

			int index = (int)find_last_of('\\');

			if( index != -1)
				r = substr( 0, index + 1);

			return r;
		}

		inline sString GetFileExt()
		{
			sString r;

			int index = (int)find_last_of('.');

			if( index != -1)
				r = substr( index + 1, size() - index - 1);

			return r;
		}

		inline sString GetFilenameExceptPath()
		{
			sString r;

			int index = (int)find_last_of('\\');

			if( index != -1)
				r = substr( index + 1, size() - index - 1);

			return r;
		}

	};

	//-----------------------------------------------------------------------------------------------------------
	struct sSize : public SIZE
	{
		inline sSize()
		{
			memset( this,0, sizeof( sSize));
		}

		inline sSize(int ix,int iy)
		{
			cx = ix;
			cy = iy;
		}

		inline sSize( const sSize &size)
		{
			memcpy( this, &size, sizeof( sSize));
		}

		inline bool operator == (const sSize &size)
		{
			return cx == size.cx && cy == size.cy;
		}

		inline sSize operator +(const sSize &size)
		{
			sSize temp;

			temp.cx = cx + size.cx;
			temp.cy = cy + size.cy;

			return temp;
		}

		inline bool operator != (const sSize &size)
		{
			return !(cx == size.cx && cy == size.cy);
		}
	};

	//-----------------------------------------------------------------------------------------------------------
	struct sPoint : public POINT
	{
		// constructor
		inline sPoint()
		{
			memset( this, 0, sizeof( sPoint));
		}

		inline sPoint(int ix,int iy)
		{
			x = ix;
			y = iy;
		}

		inline sPoint(const sPoint &point)
		{
			memcpy( this, &point, sizeof( sPoint));
		}

		// attrib
		inline int GetLength()
		{
			//return (int)sqrt( float(x * x + y * y));
			return (int)nSQRT( float(x * x + y * y));
		}

		// operator
		inline void operator = (const sPoint &point)
		{
			memcpy( this, &point, sizeof( sPoint));
		}

		inline void operator += (const sPoint &point)
		{
			x += point.x;
			y += point.y;
		}

		inline void operator -= (const sPoint &point)
		{
			x -= point.x;
			y -= point.y;
		}

		inline void operator *= (const int scalar)
		{
			x *= scalar;
			y *= scalar;
		}

		inline void operator /= (const int scalar)
		{
			x /= scalar;
			y /= scalar;
		}

		inline sPoint operator + (const sPoint &point)
		{
			return sPoint( x + point.x, y + point.y);
		}

		inline sPoint operator - (const sPoint &point)
		{
			return sPoint( x - point.x, y - point.y);
		}

		inline sPoint operator * (const int scalar)
		{
			return sPoint( x * scalar, y * scalar);
		}

		inline sPoint operator / (const int scalar)
		{
			return sPoint( x / scalar, y / scalar);
		}

		inline bool operator == (const sPoint &point)
		{
			return x == point.x && y == point.y;
		}

		inline bool operator != (const sPoint &point)
		{
			return !(x == point.x && y == point.y);
		}
	};

	//-----------------------------------------------------------------------------------------------------------
	struct sRect : public RECT
	{
		// constructor
		inline sRect()
		{
			memset( this, 0, sizeof( sRect));
		}

		inline sRect(const RECT &r)
		{
			memcpy( this, &r, sizeof( RECT));
		};

		inline sRect(const sRect &r)
		{
			memcpy( this, &r, sizeof( sRect));	
		};

		inline sRect(int ileft,int itop,int iright,int ibottom)
		{
			left = ileft;
			top = itop;
			right = iright;
			bottom = ibottom;
		}

		inline sRect(sPoint &pos,sSize &size)
		{
			left = pos.x;
			top = pos.y;
			right = pos.x + size.cx;
			bottom = pos.y + size.cy;
		}

		// attribute
		inline sSize Size()
		{
			return sSize( right - left + 1, bottom - top + 1);
		}

		inline sPoint Center()
		{
			return sPoint( (right + left) / 2, (bottom + top) / 2);
		}
		
		inline int Width()
		{
			return right - left + 1;
		}

		inline int Height()
		{
			return bottom - top + 1;
		}

		inline sRect GetLocal()
		{
			return sRect( 0, 0, right - left, bottom - top);
		}

		// operator
		inline void operator = (const sRect &rect)
		{
			memcpy( this, &rect, sizeof( sRect));
		}

		inline void operator += (const sPoint &point)
		{
			left += point.x;
			top += point.y;
			right += point.x;
			bottom += point.y;
		}
 
		inline void operator -= (const sPoint &point)
		{
			left -= point.x;
			top -= point.y;
			right -= point.x;
			bottom -= point.y;
		}

		inline bool operator == (const sRect &rect)
		{
			return left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom;
		}

		inline bool operator != (const sRect &rect)
		{
			return !(left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom);
		}

		inline bool IsIntersect(const sRect &rect)
		{
			if( left > rect.right ||
				right < rect.left ||
				top > rect.bottom ||
				bottom < rect.top)
				return FALSE;

			return TRUE;
		}

		inline bool PtInRect(const sPoint &point,bool bReCalc = TRUE )
		{
			if ( bReCalc )
			{
				//HT_CHEAT : WINDOWSIZE			
				int nLeft   = left   * XiahGameEngine::WindowRect.right  / G_WIDTH;
				int nRight  = right  * XiahGameEngine::WindowRect.right  / G_WIDTH;
				int nTop    = top    * XiahGameEngine::WindowRect.bottom / G_HEIGHT;
				int nBottom = bottom * XiahGameEngine::WindowRect.bottom / G_HEIGHT;						
				
				return point.x >= nLeft && point.x <= nRight && point.y >= nTop && point.y <= nBottom;
			}
			return point.x >= left && point.x <= right && point.y >= top && point.y <= bottom;
		}
	};

	//------------------------------------------------------------------------------------------//
	// 마우스 이벤트 값
	struct sMouseEvent
	{
		POINT ptMousePos; // 현재 마우스 좌표
		bool bLButton; // mouse L Button Down
		bool bRButton; // mouse R Button Down
		int nEventType; 
		int nEventFrameID, nEventCtrlID; // 이벤트 발생 프레임, 컨트롤
		DWORD dwTime;	// timeGetTime

		inline sMouseEvent(void) : bLButton(false), bRButton(false), nEventType(0), nEventFrameID(-1), nEventCtrlID(-1), dwTime(0)
		{
			ptMousePos.x = 0;
			ptMousePos.y = 0;
		}

		inline void operator = (const sMouseEvent &value)
		{
			memcpy(this, &value, sizeof(sMouseEvent));
		}
	};

	struct sFrameData
	{
		int nID;
		int nWidth, nHeight;
		int nResID;
		int nCtrlCount;
		int nAlpha;

		inline sFrameData() : nID(0), nWidth(0), nHeight(0), nResID(0), nCtrlCount(0), nAlpha(0)
		{
		}
	};

	struct sCtrlData
	{
		int nID;
		int nType;
		int nX, nY;
		int nWidth, nHeight;		
		int nResID, nResID2, nResID3;
		int nAlpha;

		float fU,fV;

		int nCount;			// 컨트롤 수
		std::vector<sCtrlData*> vCtrlList; // 정적 리스트 때문

		inline sCtrlData() : nID(0), nX(0), nY(0), nWidth(0), nHeight(0),  nType(0), nResID(0), nResID2(0), nResID3(0), nAlpha(0), nCount(0),
			fU(0.0f), fV(0.0f)
		{

		}

		inline void CtrlRelease()
		{
			if(nCount)
			{
				std::vector<sCtrlData*>::iterator iterlist = vCtrlList.begin();

				for(; iterlist != vCtrlList.end(); ++iterlist)
				{
					sCtrlData *pCtrlTemp = *iterlist;

					delete pCtrlTemp, pCtrlTemp = NULL;
				} // for(; iterlist != vCtrlList.end(); ++iterlist)

				vCtrlList.clear();
			}
		}
	};

};