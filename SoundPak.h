#pragma once

namespace XiahGameEngine
{
	namespace XiahSoundPak
	{
		extern XIAHGE_API BOOL InitializeSoundPak(LPCTSTR *pPakList,int count);
		extern XIAHGE_API BOOL UninitializeSoundPak();

		/*
			여기에 있는 GetSoundBuffer는 지금 당장 Play할 Sound를 얻어내는것이다.

			내부적으로 패키지로부터 데이터를 만들어 낸다음에 Duplicate해서 만들어 낸 데이터 이다

			따라서 사용하고 나면 반드시 Release해준다
		*/
		extern XIAHGE_API BOOL GetSoundBufferInstance(int nSoundID,FSOUND_SAMPLE** ppBuffer,int *pDelayTime);
		extern XIAHGE_API BOOL DelSoundBufferInstance(int nSoundID);
	};
};