#pragma once
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#pragma warning(disable : 4309)

namespace XiahGameEngine
{

	//---------------------------------------------------------------------------------------
	enum ePixelFormat
	{
		//------ Render Target Format
		PF_R5G6B5,
		PF_X1R5G5B5,
		PF_A1R5G5B5,
		PF_A4R4G4B4,
		PF_X8R8G8B8,
		PF_A8R8G8B8,
		PF_R8G8B8,
		PF_P8,			// 8Bit Indexed

		//------ Unknown Format
		PF_Unknown
	};

	const char g_PixelFormatBPP[ PF_Unknown] =
	{
		2,//PF_R5G6B5,
		2,//PF_X1R5G5B5,
		2,//PF_A1R5G5B5,
		2,//PF_A4R4G4B4,
		4,//PF_X8R8G8B8,
		4,//PF_A8R8G8B8,
		3,//PF_R8G8B8,
		1//PF_P8
	};

	const unsigned char g_PixelFormatInfo[ PF_Unknown][ 4][ 3] = 
	{
		//PF_R5G6B5
		{
			{ -1, -1, 0xff},
			{ 11,  5, 0x1f},
			{  6,  6, 0x3f},
			{  0,  5, 0x1f}
		},
		//PF_X1R5G5B5
		{
			{ -1, -1, 0xff},
			{ 10,  5, 0x1f},
			{  5,  5, 0x1f},
			{  0,  5, 0x1f}
		},
		//PF_A1R5G5B5
		{
			{ 15,  1, 0x01},
			{ 10,  5, 0x1f},
			{  5,  5, 0x1f},
			{  0,  5, 0x1f}
		},
		//PF_A4R4G4B4
		{
			{ 12,  4, 0x0f},
			{  8,  4, 0x0f},
			{  4,  4, 0x0f},
			{  0,  4, 0x0f}
		},
		//PF_X8R8G8B8
		{
			{ -1, -1, 0xff},
			{ 16,  8, 0xff},
			{  8,  8, 0xff},
			{  0,  8, 0xff}
		},
		//PF_A8R8G8B8
		{
			{ 24,  8, 0xff},
			{ 16,  8, 0xff},
			{  8,  8, 0xff},
			{  0,  8, 0xff}
		},
		//PF_R8G8B8
		{
			{ -1, -1, 0xff},
			{ 16,  8, 0xff},
			{  8,  8, 0xff},
			{  0,  8, 0xff}
		},
		//PF_P8
		{
			{ -1, -1, 0xff},
			{ -1, -1, 0xff},
			{ -1, -1, 0xff},
			{ -1, -1, 0xff}
		}
	};

	#define Dispatch_Color( data, format)	\
			argb[ 0] = argb[ 1] = argb[ 2] = argb[ 3] = 0xff;\
			if( g_PixelFormatInfo[ format][ 0][ 0] != (unsigned char)0xff)\
				argb[ 0] = ((data) >> g_PixelFormatInfo[ format][ 0][ 0]) & g_PixelFormatInfo[ format][ 0][ 2];\
			if( g_PixelFormatInfo[ format][ 1][ 0] != (unsigned char)0xff)\
				argb[ 1] = ((data) >> g_PixelFormatInfo[ format][ 1][ 0]) & g_PixelFormatInfo[ format][ 1][ 2];\
			if( g_PixelFormatInfo[ format][ 2][ 0] != (unsigned char)0xff)\
				argb[ 2] = ((data) >> g_PixelFormatInfo[ format][ 2][ 0]) & g_PixelFormatInfo[ format][ 2][ 2];\
			if( g_PixelFormatInfo[ format][ 3][ 0] != (unsigned char)0xff)\
				argb[ 3] = ((data) >> g_PixelFormatInfo[ format][ 3][ 0]) & g_PixelFormatInfo[ format][ 3][ 2];

	// 지금 보면 X8R8G8B8에서 X8부분에 데이터를 전혀 쓰지 않도록 되어 있기 때문에. 문제가 될 수도 있겠다
	#define Combine_Color( data, format)	\
			data = 0;\
			if( g_PixelFormatInfo[ format][ 0][ 0] != (unsigned char)0xff)\
				data |= (argb[ 0] & g_PixelFormatInfo[ format][ 0][ 2]) << g_PixelFormatInfo[ format][ 0][ 0];\
			if( g_PixelFormatInfo[ format][ 1][ 0] != (unsigned char)0xff)\
				data |= (argb[ 1] & g_PixelFormatInfo[ format][ 1][ 2]) << g_PixelFormatInfo[ format][ 1][ 0];\
			if( g_PixelFormatInfo[ format][ 2][ 0] != (unsigned char)0xff)\
				data |= (argb[ 2] & g_PixelFormatInfo[ format][ 2][ 2]) << g_PixelFormatInfo[ format][ 2][ 0];\
			if( g_PixelFormatInfo[ format][ 3][ 0] != (unsigned char)0xff)\
				data |= (argb[ 3] & g_PixelFormatInfo[ format][ 3][ 2]) << g_PixelFormatInfo[ format][ 3][ 0];\

	#define Rescale_ColorElement(srcFormat, destFormat)	\
			if( g_PixelFormatInfo[ srcFormat][ 0][ 1] != (unsigned char)0xff && g_PixelFormatInfo[ destFormat][ 0][ 1])\
				argb[ 0] = argb[ 0] * g_PixelFormatInfo[ destFormat][ 0][ 1] / g_PixelFormatInfo[ srcFormat][ 0][ 1];\
			if( g_PixelFormatInfo[ srcFormat][ 1][ 1] != (unsigned char)0xff && g_PixelFormatInfo[ destFormat][ 1][ 1])\
				argb[ 1] = argb[ 1] * g_PixelFormatInfo[ destFormat][ 1][ 1] / g_PixelFormatInfo[ srcFormat][ 1][ 1];\
			if( g_PixelFormatInfo[ srcFormat][ 2][ 1] != (unsigned char)0xff && g_PixelFormatInfo[ destFormat][ 2][ 1])\
				argb[ 2] = argb[ 2] * g_PixelFormatInfo[ destFormat][ 2][ 1] / g_PixelFormatInfo[ srcFormat][ 2][ 1];\
			if( g_PixelFormatInfo[ srcFormat][ 3][ 1] != (unsigned char)0xff && g_PixelFormatInfo[ destFormat][ 3][ 1])\
				argb[ 3] = argb[ 3] * g_PixelFormatInfo[ destFormat][ 3][ 1] / g_PixelFormatInfo[ srcFormat][ 3][ 1];

	inline bool IsTrueColor(ePixelFormat nFormat)
	{
		return nFormat != PF_P8 && nFormat != PF_Unknown;
	}
	
	inline void GetPixelElement(ePixelFormat nFormat,LPBYTE pData,unsigned char *argb)
	{
		if( !IsTrueColor( nFormat))	// Only Support TrueColor Format
			return;

		unsigned short *pShort = (unsigned short *)pData;
		unsigned long *pDword = (unsigned long *)pData;

		switch( g_PixelFormatBPP[ nFormat])
		{
		case 2:
			Dispatch_Color( *pShort, nFormat);
			break;
		case 3:
		case 4:
			Dispatch_Color( *pDword, nFormat);
			break;
		}
	}

	inline void PutPixelElement(ePixelFormat nFormat,LPBYTE pData,unsigned char *argb)
	{
		if( !IsTrueColor( nFormat))	// Only Support TrueColor Format
			return;

		unsigned short *pShort = (unsigned short *)pData;
		unsigned long *pDword = (unsigned long *)pData;

		switch( g_PixelFormatBPP[ nFormat])
		{
		case 2:
			Combine_Color( *pShort, nFormat);
			break;
		case 3:
		case 4:
			Combine_Color( *pDword, nFormat);
			break;
		}
	}
	
	inline void ConvertPixel(LPBYTE pSrcData,LPBYTE pDestData,ePixelFormat nSrcFormat,ePixelFormat nDestFormat)
	{
		if( !IsTrueColor( nSrcFormat) || !IsTrueColor( nDestFormat))
			return;

		unsigned char argb[ 4];

		// BPP가 다를 수 있기 때문에 Src와 Dest의 포인터를 분리하였다

		GetPixelElement( nSrcFormat, pSrcData, &argb[ 0]);

		Rescale_ColorElement( nSrcFormat, nDestFormat);

		PutPixelElement( nDestFormat, pDestData, &argb[ 0]);
	}
	
	//---------------------------------------------------------------------------------------
	typedef unsigned long P_COLOR;

	//---------------------------------------------------------------------------------------
	struct sColor
	{
		union
		{
			P_COLOR color;
			struct
			{
				// IBM은 꺼꾸로 라네~
				unsigned char b;
				unsigned char g;
				unsigned char r;
				unsigned char a;
			};
		};

	//----------------------
		inline sColor()
		{
			memset( this, 0, sizeof( sColor));
		}
		
		inline sColor(const P_COLOR &c)
		{
			color = c;
		}

		inline sColor(int ia,int ir,int ig,int ib)
		{
			a = ia;
			r = ir;
			g = ig;
			b = ib;
		}

		inline sColor(int ir,int ig,int ib)
		{
			a = 255;
			r = ir;
			g = ig;
			b = ib;
		}

		inline sColor(float fa,float fr,float fg,float fb)
		{
			a = (unsigned char)( fa * 255);
			r = (unsigned char)( fr * 255);
			g = (unsigned char)( fg * 255);
			b = (unsigned char)( fb * 255);
		}
		
		inline sColor(float fr,float fg,float fb)
		{
			a = 255;
			r = (unsigned char)( fr * 255);
			g = (unsigned char)( fg * 255);
			b = (unsigned char)( fb * 255);
		}

		inline operator P_COLOR()
		{
			return color;
		}
	};
};

#pragma warning(pop)

