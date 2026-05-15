#include "stdafx.h"
#include "Trigger.h"

namespace XiahGameEngine
{

	/*************************************************************************************************************
	..............................................................................................................
	......................SSSS...EEEEEE..PPPPP.....AA....RRRRR.....AA....TTTTTT...OOOO...RRRRR....................
	.....................SS..SS..EE......PP..PP...AAAA...RR..RR...AAAA.....TT....OO..OO..RR..RR...................
	.....................SS......EE......PP..PP..AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
	......................SSSS...EEEEEE..PPPPP...AAAAAA..RRRR....AAAAAA....TT....OO..OO..RRRR.....................
	.........................SS..EE......PP......AA..AA..RR.RR...AA..AA....TT....OO..OO..RR.RR....................
	.....................SS..SS..EE......PP......AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
	......................SSSS...EEEEEE..PP......AA..AA..RR..RR..AA..AA....TT.....OOOO...RR..RR...................
	..............................................................................................................
	*************************************************************************************************************/
	XIAHGE_API CTriggerList::CTriggerList()
	{
	}
	
	XIAHGE_API CTriggerList::~CTriggerList()
	{
		// Trigger Instance객체는 지워주지 않는다
		clear();
	}

	XIAHGE_API BOOL CTriggerList::SetSize(int nCount)
	{
		resize( nCount);
		for(int i = 0; i < nCount; i++)
		{
			CTrigger*& pTrigger = operator [](i);

			pTrigger = NULL;
		}

		return TRUE;
	} 
	
	XIAHGE_API BOOL CTriggerList::SetTrigger(int nIndex,CTrigger *pT)
	{
		CTrigger*& pTrigger = operator[]( nIndex);

		pTrigger = pT;

		return TRUE;
	}
	
	XIAHGE_API int CTriggerList::Invoke(int nIndex)
	{
		CTrigger*& pTrigger = operator[](nIndex);

		if( pTrigger == NULL)
		{
			DBG_LogFile( _T("CTriggerList::Invoke fail"));
			return -1;
		}

		return pTrigger->Invoke();
	}

	XIAHGE_API int CTriggerList::Invoke(int nIndex,unsigned long param)
	{
		CTrigger*& pTrigger = operator[](nIndex);

		if( pTrigger == NULL)
		{
			DBG_LogFile( _T("CTriggerList::Invoke2 fail"));
			return -1;
		}

		pTrigger->m_nParam = param;

		return pTrigger->Invoke();
	}
};