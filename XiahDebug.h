#pragma once

#include <tchar.h>
#include <stdio.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#ifndef MASTER

	#include <dbghelp.h>
	enum BasicType  
	{
		btNoType = 0,
		btVoid = 1,
		btChar = 2,
		btWChar = 3,
		btInt = 6,
		btUInt = 7,
		btFloat = 8,
		btBCD = 9,
		btBool = 10,
		btLong = 13,
		btULong = 14,
		btCurrency = 25,
		btDate = 26,
		btVariant = 27,
		btComplex = 28,
		btBit = 29,
		btBSTR = 30,
		btHresult = 31
	};


	class cExceptionReport
	{
	public:
		cExceptionReport( );
		~cExceptionReport( );

		void SetLogFileName( PTSTR pszLogFileName );
		static LONG WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo );

	private:

		static void GenerateExceptionReport( PEXCEPTION_POINTERS pExceptionInfo );
		static LPTSTR GetExceptionString( DWORD dwCode );
		static BOOL GetLogicalAddress(  PVOID addr, PTSTR szModule, DWORD len,DWORD& section, DWORD& offset );
		static void WriteStackDetails( PCONTEXT pContext, bool bWriteVariables );
		static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);
		static bool FormatSymbolValue( PSYMBOL_INFO, STACKFRAME *, char * pszBuffer, unsigned cbBuffer );
		static char * DumpTypeIndex( char *, DWORD64, DWORD, unsigned, DWORD_PTR, bool & );
		static char * FormatOutputValue( char * pszCurrBuffer, BasicType basicType, DWORD64 length, PVOID pAddress );
		static BasicType GetBasicType( DWORD typeIndex, DWORD64 modBase );
		static int __cdecl _tprintf(const TCHAR * format, ...);

		static TCHAR m_szLogFileName[MAX_PATH];
		static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
		static HANDLE m_hReportFile;
		static HANDLE m_hProcess;
	};


	extern cExceptionReport g_cExceptionReport;

#endif

//-----------------------------------------------------------------------------------------------------------
// Function이름을 주면 Function이름과 함께 메세지 출력
inline void DBG_PutFunction(const TCHAR *function_name,const TCHAR *format,...)
{
#ifdef _DEBUG
	char buff_format[ 128];
	char buff[ 255];

	va_list ap;

	va_start( ap, format);
	vsprintf( buff_format, format, ap);
	va_end( ap);

	_stprintf( buff, "DebugOut : Function \"%s\" : %s\n", function_name, buff_format);

	::OutputDebugString( buff);
#endif
}

//-----------------------------------------------------------------------------------------------------------
// Output창에 Debug메세지 출력
inline void DBG_Put(TCHAR *format,...)
{
#ifdef _DEBUG
	TCHAR buff[ 255];

	va_list ap;
	
	va_start( ap, format);
	vsprintf( buff, format, ap);
	va_end( ap);

	::OutputDebugString( buff);
	::OutputDebugString( "\n");
#endif
}

inline void DBG_LogFile(TCHAR *format,...)
{
#ifndef MASTER
	static int Log_count = 0;
	TCHAR buff[ 255];
	va_list ap;
	
	va_start( ap, format);
	_vstprintf( buff, format, ap);
	va_end( ap);

	SYSTEMTIME lpSystemTime;
	GetLocalTime(&lpSystemTime);

	FILE *fp;
	fp = _tfopen( _T("Xiah.log"), _T("a"));
	_ftprintf(fp, _T("------------------------------------------------(%d)\n"),Log_count);
	_ftprintf(fp, _T("%d/%d/%d %d:%d:%d\n"),lpSystemTime.wYear,lpSystemTime.wMonth,lpSystemTime.wDay,lpSystemTime.wHour,lpSystemTime.wMinute,lpSystemTime.wSecond);
	_ftprintf(fp, _T("%s\n"), buff);
	fclose( fp);
	Log_count++;
#endif
}

//HT_TEST
inline void DBG_LogFile_TEST(TCHAR *format,...)
{
#ifndef MASTER
	static int Log_count = 0;
	TCHAR buff[ 255];
	va_list ap;
	
	va_start( ap, format);
	_vstprintf( buff, format, ap);
	va_end( ap);

	SYSTEMTIME lpSystemTime;
	GetLocalTime(&lpSystemTime);

	FILE *fp;
	fp = _tfopen( _T("Xiah_TEST.log"), _T("a"));
	_ftprintf(fp, _T("------------------------------------------------(%d)\n"),Log_count);
	_ftprintf(fp, _T("%d/%d/%d %d:%d:%d\n"),lpSystemTime.wYear,lpSystemTime.wMonth,lpSystemTime.wDay,lpSystemTime.wHour,lpSystemTime.wMinute,lpSystemTime.wSecond);
	_ftprintf(fp, _T("%s\n"), buff);
	fclose( fp);
	Log_count++;
#endif
}

inline void DBG_LogFile_CloseXiah(TCHAR *format,...)
{
#ifndef MASTER
	static int Log_count = 0;
	TCHAR buff[ 255];
	va_list ap;
	
	va_start( ap, format);
	_vstprintf( buff, format, ap);
	va_end( ap);

	SYSTEMTIME lpSystemTime;
	GetLocalTime(&lpSystemTime);

	FILE *fp;
	fp = _tfopen( _T("Xiah_Close.log"), _T("a"));
	_ftprintf(fp, _T("------------------------------------------------(%d)\n"),Log_count);
	_ftprintf(fp, _T("%d/%d/%d %d:%d:%d\n"),lpSystemTime.wYear,lpSystemTime.wMonth,lpSystemTime.wDay,lpSystemTime.wHour,lpSystemTime.wMinute,lpSystemTime.wSecond);
	_ftprintf(fp, _T("%s\n"), buff);
	fclose( fp);
	Log_count++;
#endif
}

//-----------------------------------------------------------------------------------------------------------
// Debugging용 Assert매크로 
// expression이 FALSE가 되면 Assertion Fail이 일어남
#ifdef _DEBUG
#define DBG_Assert( expression)	\
	if( !(expression))\
	{\
		DBG_Put(_T(""));\
		DBG_PutFunction( __FUNCTION__, #expression);\
		DBG_Put(_T(""));\
		_CrtDbgBreak();\
	}
#else
#define DBG_Assert( expression)
#endif

#define DBG_BREAK _CrtDbgBreak()


