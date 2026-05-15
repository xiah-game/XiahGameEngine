// XiahGameEngine.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "cpu\\cpu.h"
#include "cpu\\optimize.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	// Generate CPU Optimize
	XiahGameEngine::GetCPUInformation();

    return TRUE;
}

