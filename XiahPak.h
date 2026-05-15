#pragma once

namespace XiahGameEngine
{
	namespace XiahPak
	{
		XIAHGE_API extern BOOL InitializeXiahPak(LPCTSTR *pPakFileList,unsigned long count);
		XIAHGE_API extern BOOL UninitializeXiahPak();

		XIAHGE_API extern BOOL ReleaseAllResource();
		XIAHGE_API extern BOOL ReleaseRes(int nResID);

		XIAHGE_API extern IDirect3DTexture9 *GetTexture(int nResID,BOOL bPreserveQuality = FALSE);

		// FMOD
		XIAHGE_API extern FSOUND_SAMPLE *GetSound(int nResID,BOOL bPreserveQuality = FALSE);
		// DSOUND
		//XIAHGE_API extern IDirectSoundBuffer8 *GetSound(int nResID,BOOL bPreserveQuality = FALSE);
	};
};