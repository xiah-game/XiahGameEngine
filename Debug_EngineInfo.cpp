#include "stdafx.h"
#include "Debug_EngineInfo.h"
#include "Text2D.h"

namespace XiahGameEngine
{
	namespace DebugEngineInfo
	{
	#ifdef _DEBUG	
		BOOL g_bShowDebugEngineInfo = TRUE;
	#else
		BOOL g_bShowDebugEngineInfo = FALSE;
	#endif

		XIAHGE_API BOOL ShowDebugEngineInfo(BOOL bShow)
		{
			g_bShowDebugEngineInfo = bShow;
			return TRUE;
		}

		XIAHGE_API BOOL DrawDebugEngineInfo()
		{
			if( g_bShowDebugEngineInfo == FALSE)
				return TRUE;

			return TRUE;
		}

		XIAHGE_API BOOL ReleaseDebugEngineInfo()
		{
			return TRUE;
		}
	};
};