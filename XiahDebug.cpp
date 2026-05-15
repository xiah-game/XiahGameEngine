#ifndef MASTER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <dbghelp.h>
#include "xiahdebug.h"

#pragma comment(linker, "/defaultlib:dbghelp.lib")

LPTOP_LEVEL_EXCEPTION_FILTER cExceptionReport::m_previousFilter;
TCHAR cExceptionReport::m_szLogFileName[MAX_PATH];
HANDLE cExceptionReport::m_hReportFile;
HANDLE cExceptionReport::m_hProcess;

cExceptionReport g_cExceptionReport;

// 전역 변수 까지 뽑을려면 사용
//#define	_OUTPUTGROVAL_

//////////////////////////////////////////////////////////////////////////

cExceptionReport::cExceptionReport()
{
	// 여기서 Crash시 report할것을 결정
    m_previousFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
    GetModuleFileName( 0, m_szLogFileName, MAX_PATH );

    PTSTR pszDot = _tcsrchr( m_szLogFileName, _T('.') );
    if ( pszDot )
    {
        pszDot++;
        if ( _tcslen(pszDot) >= 3 )
            _tcscpy( pszDot, _T("LOG") );	// 로그의 확장자
    }

    m_hProcess = GetCurrentProcess();
}

cExceptionReport::~cExceptionReport( )
{
	// 해제
    SetUnhandledExceptionFilter(m_previousFilter );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cExceptionReport::SetLogFileName( PTSTR pszLogFileName )
{
    _tcscpy( m_szLogFileName, pszLogFileName );
}


LONG WINAPI cExceptionReport::UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo )
{
    m_hReportFile = CreateFile( m_szLogFileName,
                                GENERIC_WRITE,
                                0,
                                0,
                                OPEN_ALWAYS,
                                FILE_FLAG_WRITE_THROUGH,
                                0 );

    if ( m_hReportFile )
    {
        SetFilePointer( m_hReportFile, 0, 0, FILE_END );

        GenerateExceptionReport( pExceptionInfo );

        CloseHandle( m_hReportFile );
        m_hReportFile = 0;
    }

    if ( m_previousFilter )
        return m_previousFilter( pExceptionInfo );
    else
        return EXCEPTION_CONTINUE_SEARCH;
}

void cExceptionReport::GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo )
{

	SYSTEMTIME	time;
	GetLocalTime(&time);
	// 시간을 남기고.
	_tprintf(_T("=====================================================\r\n"));
	_tprintf(_T("%d월 %d일 %d시 %d분 %d초\r\n"),time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
    _tprintf(_T("=====================================================\r\n"));

    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

	_tprintf(_T("오류코드 : %08X 에러종류 : %s\r\n"),pExceptionRecord->ExceptionCode,GetExceptionString(pExceptionRecord->ExceptionCode) );

    TCHAR szFaultingModule[MAX_PATH];
    DWORD section, offset;
    GetLogicalAddress(  pExceptionRecord->ExceptionAddress,szFaultingModule,sizeof( szFaultingModule ), section, offset );

    _tprintf( _T("오류주소 :  %08X %02X:%08X %s\r\n"), pExceptionRecord->ExceptionAddress,section, offset, szFaultingModule );

    PCONTEXT pCtx = pExceptionInfo->ContextRecord;

	// X86 계열
    #ifdef _M_IX86  // X86 Only!
	_tprintf( _T("\r\n[ 레지스터 내용 ]\r\n") );

	// 범용 레지스터
    _tprintf(_T("EAX:%08X EBX:%08X ECX:%08X EDX:%08X\r\nESI:%08X EDI:%08X\r\n")
            ,pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx,pCtx->Esi, pCtx->Edi );

	// 특수
    _tprintf( _T("CS:EIP:%04X:%08X\r\n"), pCtx->SegCs, pCtx->Eip );
    _tprintf( _T("SS:ESP:%04X:%08X  EBP:%08X\r\n"),
                pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
    _tprintf( _T("DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n"),
                pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs );
	_tprintf( _T("[ 플레그 ] : %08X\r\n\r\n"), pCtx->EFlags );

    #endif

    SymSetOptions( SYMOPT_DEFERRED_LOADS );

    // Initialize DbgHelp
    if ( !SymInitialize( GetCurrentProcess(), 0, TRUE ) )
        return;

    CONTEXT trashableContext = *pCtx;

	//HT_TEST
	DBG_LogFile( _T("cExceptionReport::WriteStackDetails (시작)"));
    WriteStackDetails( &trashableContext, false );
	DBG_LogFile( _T("cExceptionReport::WriteStackDetails (종료)"));

    #ifdef _M_IX86  // X86 Only!

    _tprintf(_T("=====================================================\r\n"));
	_tprintf(_T("[ 지역 변수와 파라메터 정보 ]\r\n") );

    trashableContext = *pCtx;
    WriteStackDetails( &trashableContext, true );

    _tprintf(_T("=====================================================\r\n"));

// 전역변수를 출력!
#ifdef _OUTPUTGROVAL_
    _tprintf( _T("전역변수\r\n") );
    SymEnumSymbols( GetCurrentProcess(), (DWORD64)GetModuleHandle(szFaultingModule),0, EnumerateSymbolsCallback, 0 );
#endif

    #endif      // X86 Only!

    SymCleanup( GetCurrentProcess() );
    _tprintf( _T("\r\n") );

	MessageBox(GetDesktopWindow(),"오류가 발생하였습니다.\n오류는 XiahClient.log에 기록됩니다.","Fatal Error!",MB_OK);
}

LPTSTR cExceptionReport::GetExceptionString( DWORD dwCode )
{
    #define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);

    switch ( dwCode )
    {
        EXCEPTION( ACCESS_VIOLATION )
        EXCEPTION( DATATYPE_MISALIGNMENT )
        EXCEPTION( BREAKPOINT )
        EXCEPTION( SINGLE_STEP )
        EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
        EXCEPTION( FLT_DENORMAL_OPERAND )
        EXCEPTION( FLT_DIVIDE_BY_ZERO )
        EXCEPTION( FLT_INEXACT_RESULT )
        EXCEPTION( FLT_INVALID_OPERATION )
        EXCEPTION( FLT_OVERFLOW )
        EXCEPTION( FLT_STACK_CHECK )
        EXCEPTION( FLT_UNDERFLOW )
        EXCEPTION( INT_DIVIDE_BY_ZERO )
        EXCEPTION( INT_OVERFLOW )
        EXCEPTION( PRIV_INSTRUCTION )
        EXCEPTION( IN_PAGE_ERROR )
        EXCEPTION( ILLEGAL_INSTRUCTION )
        EXCEPTION( NONCONTINUABLE_EXCEPTION )
        EXCEPTION( STACK_OVERFLOW )
        EXCEPTION( INVALID_DISPOSITION )
        EXCEPTION( GUARD_PAGE )
        EXCEPTION( INVALID_HANDLE )
    }

    // If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.

    static TCHAR szBuffer[512] = { 0 };

    FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                   GetModuleHandle( _T("NTDLL.DLL") ),
                   dwCode, 0, szBuffer, sizeof( szBuffer ), 0 );

    return szBuffer;
}

BOOL cExceptionReport::GetLogicalAddress( PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset )
{
    MEMORY_BASIC_INFORMATION mbi;

    if (!VirtualQuery( addr, &mbi, sizeof(mbi))) return FALSE;
    DWORD hMod = (DWORD)mbi.AllocationBase;

    if (!GetModuleFileName( (HMODULE)hMod, szModule, len )) return FALSE;
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );
    DWORD rva = (DWORD)addr - hMod; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for (   unsigned i = 0;
            i < pNtHdr->FileHeader.NumberOfSections;
            i++, pSection++ )
    {
        DWORD sectionStart = pSection->VirtualAddress;
        DWORD sectionEnd = sectionStart
                    + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section???
        if ( (rva >= sectionStart) && (rva <= sectionEnd) )
        {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = rva - sectionStart;
            return TRUE;
        }
    }

    return FALSE;   // Should never get here!
}

//============================================================
// Walks the stack, and writes the results to the report file 
//============================================================
// bWriteVariables : true if local/params should be output
void cExceptionReport::WriteStackDetails(PCONTEXT pContext,bool bWriteVariables )
{
    //_tprintf( _T("\r\nCall stack:\r\n") );
    _tprintf( _T("Address   Frame     Function            SourceFile\r\n") );

    DWORD dwMachineType = 0;
    // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

    STACKFRAME sf;
    memset( &sf, 0, sizeof(sf) );

    #ifdef _M_IX86
    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    sf.AddrPC.Offset       = pContext->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;
    #endif

 //   while (1)
   // {
        // Get the next stack frame
        StackWalk  (  dwMachineType,				m_hProcess,   GetCurrentThread(),   &sf,  pContext,  0,  
					  SymFunctionTableAccess,    SymGetModuleBase, 0 );

		 // Basic sanity check to make sure the frame is OK.  Bail if not.
        if ( 0 == sf.AddrFrame.Offset )
            return;

        _tprintf( _T("%08X  %08X  "), sf.AddrPC.Offset, sf.AddrFrame.Offset );

        // Get the name of the function for this stack frame entry
        BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(symbolBuffer);
        pSymbol->MaxNameLen = 1024;
                        
        DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                        // relative to the start of the symbol

        if ( SymFromAddr(m_hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol))
        {
            _tprintf( _T("%hs+%I64X"), pSymbol->Name, symDisplacement );
            
        }
        else
        {
			// 심벌을 찾지 못할경우, Address로 대신 출력
            TCHAR szModule[MAX_PATH] = _T("");
            DWORD section = 0, offset = 0;

            GetLogicalAddress(  (PVOID)sf.AddrPC.Offset,
                                szModule, sizeof(szModule), section, offset );

            _tprintf( _T("%04X:%08X %s"), section, offset, szModule );
        }

        // Get the source line for this stack frame entry
        IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
        DWORD dwLineDisplacement;
        if ( SymGetLineFromAddr( m_hProcess, sf.AddrPC.Offset,&dwLineDisplacement, &lineInfo ) )
        {
			_tprintf(_T("[파일:%s] [라인:%u]"),lineInfo.FileName,lineInfo.LineNumber); 
        }
        _tprintf( _T("\r\n") );

        // 변수 출력
        if ( bWriteVariables )
        {
            // Use SymSetContext to get just the locals/params for this frame
            IMAGEHLP_STACK_FRAME imagehlpStackFrame;
            imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
            SymSetContext( m_hProcess, &imagehlpStackFrame, 0 );

            // Enumerate the locals/parameters
            SymEnumSymbols( m_hProcess, 0, 0, EnumerateSymbolsCallback, &sf );

            _tprintf( _T("\r\n") );
        }
 //   }

}

//////////////////////////////////////////////////////////////////////////////
// The function invoked by SymEnumSymbols
//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK cExceptionReport::EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,ULONG SymbolSize,PVOID UserContext )
{
    char szBuffer[2048];

    __try
    {
        if ( FormatSymbolValue( pSymInfo, (STACKFRAME*)UserContext,
                                szBuffer, sizeof(szBuffer) ) )  
            _tprintf( _T("- %s\r\n"), szBuffer );
    }
    __except( 1 )
    {
        _tprintf( _T("punting on symbol %s\r\n"), pSymInfo->Name );
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Given a SYMBOL_INFO representing a particular variable, displays its
// contents.  If it's a user defined type, display the members and their
// values.
//////////////////////////////////////////////////////////////////////////////
bool cExceptionReport::FormatSymbolValue(PSYMBOL_INFO pSym,STACKFRAME * sf,char * pszBuffer,unsigned cbBuffer )
{
    char * pszCurrBuffer = pszBuffer;

    // Indicate if the variable is a local or parameter
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
		pszCurrBuffer += sprintf( pszCurrBuffer, "파라메터: " );
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
		pszCurrBuffer += sprintf( pszCurrBuffer, "지역변수: " );

    // If it's a function, don't do anything.
    if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
        return false;

    // Emit the variable name
    pszCurrBuffer += sprintf( pszCurrBuffer, "\'%s\'", pSym->Name );

    DWORD_PTR pVariable = 0;    // Will point to the variable's data in memory

    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE )
    {
        // if ( pSym->Register == 8 )   // EBP is the value 8 (in DBGHELP 5.1)
        {                               //  This may change!!!
            pVariable = sf->AddrFrame.Offset;
            pVariable += (DWORD_PTR)pSym->Address;
        }
        // else
        //  return false;
    }
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
    {
        return false;   // Don't try to report register variable
    }
    else
    {
        pVariable = (DWORD_PTR)pSym->Address;   // It must be a global variable
    }

    // Determine if the variable is a user defined type (UDT).  IF so, bHandled
    // will return true.
    bool bHandled;
    pszCurrBuffer = DumpTypeIndex(pszCurrBuffer,pSym->ModBase, pSym->TypeIndex,
                                    0, pVariable, bHandled );

    if ( !bHandled )
    {
        // The symbol wasn't a UDT, so do basic, stupid formatting of the
        // variable.  Based on the size, we're assuming it's a char, WORD, or
        // DWORD.
        BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
        
        pszCurrBuffer = FormatOutputValue(pszCurrBuffer, basicType, pSym->Size,(PVOID)pVariable ); 
    }


    return true;
}

//////////////////////////////////////////////////////////////////////////////
// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types.  When he hit fundamental types, return
// bHandled = false, so that FormatSymbolValue() will format them.
//////////////////////////////////////////////////////////////////////////////
char * cExceptionReport::DumpTypeIndex(
        char * pszCurrBuffer,
        DWORD64 modBase,
        DWORD dwTypeIndex,
        unsigned nestingLevel,
        DWORD_PTR offset,
        bool & bHandled )
{
    bHandled = false;

    // Get the name of the symbol.  This will either be a Type name (if a UDT),
    // or the structure member name.
    WCHAR * pwszTypeName;
    if ( SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME,
                        &pwszTypeName ) )
    {
        pszCurrBuffer += sprintf( pszCurrBuffer, " %ls", pwszTypeName );
        LocalFree( pwszTypeName );
    }

    // Determine how many children this type has.
    DWORD dwChildrenCount = 0;
    SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT,
                    &dwChildrenCount );

    if ( !dwChildrenCount )     // If no children, we're done
        return pszCurrBuffer;

    // Prepare to get an array of "TypeIds", representing each of the children.
    // SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
    // TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
    struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
    {
        ULONG   MoreChildIds[1024];
        FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
    } children;

    children.Count = dwChildrenCount;
    children.Start= 0;

    // Get the array of TypeIds, one for each child type
    if ( !SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN,
                            &children ) )
    {
        return pszCurrBuffer;
    }

    // Append a line feed
    pszCurrBuffer += sprintf( pszCurrBuffer, "\r\n" );

    // Iterate through each of the children
    for ( unsigned i = 0; i < dwChildrenCount; i++ )
    {
        // Add appropriate indentation level (since this routine is recursive)
        for ( unsigned j = 0; j <= nestingLevel+1; j++ )
            pszCurrBuffer += sprintf( pszCurrBuffer, "\t" );

        // Recurse for each of the child types
        bool bHandled2;
        pszCurrBuffer = DumpTypeIndex( pszCurrBuffer, modBase,
                                        children.ChildId[i], nestingLevel+1,
                                        offset, bHandled2 );

        // If the child wasn't a UDT, format it appropriately
        if ( !bHandled2 )
        {
            // Get the offset of the child member, relative to its parent
            DWORD dwMemberOffset;
            SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i],
                            TI_GET_OFFSET, &dwMemberOffset );

            // Get the real "TypeId" of the child.  We need this for the
            // SymGetTypeInfo( TI_GET_TYPEID ) call below.
            DWORD typeId;
            SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i],
                            TI_GET_TYPEID, &typeId );

            // Get the size of the child member
            ULONG64 length;
            SymGetTypeInfo(m_hProcess, modBase, typeId, TI_GET_LENGTH,&length);

            // Calculate the address of the member
            DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

            BasicType basicType = GetBasicType(children.ChildId[i], modBase );

            pszCurrBuffer = FormatOutputValue( pszCurrBuffer, basicType,
                                                length, (PVOID)dwFinalOffset ); 

            pszCurrBuffer += sprintf( pszCurrBuffer, "\r\n" );
        }
    }

    bHandled = true;
    return pszCurrBuffer;
}

char * cExceptionReport::FormatOutputValue(   char * pszCurrBuffer,
                                                    BasicType basicType,
                                                    DWORD64 length,
                                                    PVOID pAddress )
{
    // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
    if ( length == 1 )
        pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PBYTE)pAddress );
    else if ( length == 2 )
        pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PWORD)pAddress );
    else if ( length == 4 )
    {
        if ( basicType == btFloat )
        {
            pszCurrBuffer += sprintf(pszCurrBuffer," = %f", *(PFLOAT)pAddress);
        }
        else if ( basicType == btChar )
        {
            if ( !IsBadStringPtr( *(PSTR*)pAddress, 32) )
            {
                pszCurrBuffer += sprintf( pszCurrBuffer, " = \"%.31s\"",
                                            *(PDWORD)pAddress );
            }
            else
                pszCurrBuffer += sprintf( pszCurrBuffer, " = %X",
                                            *(PDWORD)pAddress );
        }
        else
            pszCurrBuffer += sprintf(pszCurrBuffer," = %X", *(PDWORD)pAddress);
    }
    else if ( length == 8 )
    {
        if ( basicType == btFloat )
        {
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %lf",
                                        *(double *)pAddress );
        }
        else
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %I64X",
                                        *(DWORD64*)pAddress );
    }

    return pszCurrBuffer;
}

BasicType
cExceptionReport::GetBasicType( DWORD typeIndex, DWORD64 modBase )
{
    BasicType basicType;
    if ( SymGetTypeInfo( m_hProcess, modBase, typeIndex,
                        TI_GET_BASETYPE, &basicType ) )
    {
        return basicType;
    }

    // Get the real "TypeId" of the child.  We need this for the
    // SymGetTypeInfo( TI_GET_TYPEID ) call below.
    DWORD typeId;
    if (SymGetTypeInfo(m_hProcess,modBase, typeIndex, TI_GET_TYPEID, &typeId))
    {
        if ( SymGetTypeInfo( m_hProcess, modBase, typeId, TI_GET_BASETYPE,
                            &basicType ) )
        {
            return basicType;
        }
    }

    return btNoType;
}

//============================================================================
// Helper function that writes to the report file, and allows the user to use 
// printf style formating                                                     
//============================================================================
int __cdecl cExceptionReport::_tprintf(const TCHAR * format, ...)
{
    TCHAR szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;
          
    va_start( argptr, format );
    retValue = vsprintf( szBuff, format, argptr );
    va_end( argptr );

    WriteFile(m_hReportFile, szBuff, retValue * sizeof(TCHAR), &cbWritten, 0 );

    return retValue;
}

#endif