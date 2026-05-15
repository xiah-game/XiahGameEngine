#include "stdafx.h"
#include "SoundPak.h"
// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"

#define MAX_SOUND_FILE_SIZE	(3 * 1024 * 1024)

namespace XiahGameEngine
{
	namespace XiahSoundPak
	{
		char *g_SoundTempBuffer;

		class CSoundBuffer
		{
		public:
			CSoundBuffer()
			{
				m_nSoundID = 0;
				m_bRealize = FALSE;
				m_pSoundBuffer = NULL;
				m_nStartTime = 0;
				m_hFileHandle = NULL;
			}
			virtual ~CSoundBuffer()
			{
				Release();
			}

		public:
			int					 m_nSoundID;
			DWORD				 m_nStartTime;
			int					 m_nType;
			int					 m_nOffset;
			int					 m_nSize;

			HANDLE				 m_hFileHandle;
			BOOL				 m_bRealize;

			// FMOD
			FSOUND_SAMPLE*		m_pSoundBuffer;

			BOOL Release()
			{
				if( m_pSoundBuffer)
				{
					// FMOD (여기에서 SOUND FX는 모아서 해제해준다
					if(m_pSoundBuffer)
						FSOUND_Sample_Free(m_pSoundBuffer);

					m_pSoundBuffer = NULL;
				}
				m_bRealize = FALSE;
				m_nSoundID = 0;
				m_hFileHandle = NULL;

				return TRUE;
			}

			BOOL Realize()
			{
				if(m_bRealize) return TRUE;

				if( m_nSize > MAX_SOUND_FILE_SIZE)
				{
					throw _T("단인 Wave파일의 사이즈가 3메가가 넘습니다. 젠장!");
					return FALSE;
				}

				if( SetFilePointer( m_hFileHandle, m_nOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				{
					throw _T("파일 offset값이 잘못 되었습니다");
					return FALSE;
				}

				DWORD nReadByte;
				g_SoundTempBuffer = new char [m_nSize];

				if( !ReadFile( m_hFileHandle, g_SoundTempBuffer, m_nSize, &nReadByte, NULL))
				{
					throw _T("파일을 읽는데 실패하였습니다");
					return FALSE;
				}

				// 실제의 SOUND를 읽어온다
				// FMOD
				m_pSoundBuffer = FSOUND_Sample_Load(FSOUND_UNMANAGED,g_SoundTempBuffer, FSOUND_LOADMEMORY  | FSOUND_NORMAL | FSOUND_2D, 0, m_nSize);
				if( m_pSoundBuffer == NULL)
				{
					throw _T("Wave데이터가 잘못 되었씁니다");
					return FALSE;
				}

				delete [] g_SoundTempBuffer;

				m_bRealize = TRUE;
				return TRUE;
			}
		};

		typedef std::hash_map<int,CSoundBuffer*>	SOUNDBUFFERLIST;
		typedef std::vector<HANDLE>					FILEHANDLELIST;

		SOUNDBUFFERLIST		g_SoundBufferList;
		FILEHANDLELIST		g_FileHandleList;

		XIAHGE_API BOOL InitializeSoundPak(LPCTSTR *pPakList,int count)
		{
			try
			{
				unsigned long i;
				unsigned long j;
				DWORD nReadByte;

				for(i = 0; i < count; ++i)
				{
#ifdef _DEBUG
					DBG_Put(_T("Res Sound 로딩: %s"), pPakList[ i]);
#endif // _DEBUG

					HANDLE hFileHandle = CreateFile( pPakList[ i],
													GENERIC_READ,
													FILE_SHARE_READ,
													NULL,
													OPEN_EXISTING,
													FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
													NULL);

					if( hFileHandle == INVALID_HANDLE_VALUE)
						throw _T("Sound Pak파일이 없습니다.");

					DWORD nTemp;

					if( !ReadFile( hFileHandle, &nTemp, 4, &nReadByte, NULL))
						throw _T("Sound Pak파일 읽기 에러");
					
					// SOUND PAK에서 SOUND FX의 정보만 읽어온다.
					for(j = 0; j < nTemp; ++j)
					{
						CSoundBuffer* pSoundBuffer = new CSoundBuffer;	// 일단 동적할당이닷!

						if( !ReadFile( hFileHandle, &pSoundBuffer->m_nSoundID, 4, &nReadByte, NULL))
							throw _T("Sound Pak파일 읽기 에러");

						if( !ReadFile( hFileHandle, &pSoundBuffer->m_nStartTime, 4, &nReadByte, NULL))
							throw _T("Sound Pak파일 읽기 에러");
						
						if( !ReadFile( hFileHandle, &pSoundBuffer->m_nType, 4, &nReadByte, NULL))
							throw _T("Sound Pak파일 읽기 에러");

						if( !ReadFile( hFileHandle, &pSoundBuffer->m_nOffset, 4, &nReadByte, NULL))
							throw _T("Sound Pak파일 읽기 에러");

						if( !ReadFile( hFileHandle, &pSoundBuffer->m_nSize, 4, &nReadByte, NULL))
							throw _T("Sound Pak파일 읽기 에러");

						pSoundBuffer->m_bRealize = FALSE;
						pSoundBuffer->m_hFileHandle = hFileHandle;

						if( g_SoundBufferList.find( pSoundBuffer->m_nSoundID) != g_SoundBufferList.end())
						{
							DBG_Put(_T("Res Sound ID 중복 에러 : %d"), pSoundBuffer->m_nSoundID);
						}
						else
						{
#ifdef _DEBUG
							//DBG_Put(_T("Res Sound ID: %d"), pSoundBuffer->m_nSoundID);
#endif // _DEBUG
							g_SoundBufferList.insert( SOUNDBUFFERLIST::value_type( pSoundBuffer->m_nSoundID, pSoundBuffer));
						}						
					}

					g_FileHandleList.push_back( hFileHandle);
				}
			}
			catch(LPCTSTR strError)
			{
				MessageBox( GetForegroundWindow(), strError, _T("Sound Package"), MB_OK | MB_ICONERROR);
				return FALSE;
			}

			return TRUE;
		}

		XIAHGE_API BOOL UninitializeSoundPak()
		{
			for(SOUNDBUFFERLIST::iterator it = g_SoundBufferList.begin(); it != g_SoundBufferList.end(); ++it)
			{
				CSoundBuffer *pSoundBuffer = it->second;

				delete pSoundBuffer;
			}

			g_SoundBufferList.clear();

			for(FILEHANDLELIST::iterator f_it = g_FileHandleList.begin(); f_it != g_FileHandleList.end(); ++f_it)
			{
				HANDLE hHandle = *f_it;

				CloseHandle( hHandle);
			}

			g_FileHandleList.clear();

			return TRUE;
		}

		// FMOD
		XIAHGE_API BOOL GetSoundBufferInstance(int nSoundID, FSOUND_SAMPLE** ppBuffer, int *pDelayTime)
		{
			SOUNDBUFFERLIST::iterator it = g_SoundBufferList.find( nSoundID);

			if( it == g_SoundBufferList.end())
				return FALSE;

			CSoundBuffer *pSoundBuffer = it->second;

			try
			{
				// 실제의 데이터를 로딩 안 했으면 읽어낸다
				pSoundBuffer->Realize();
			}
			catch (LPCTSTR strMessage)
			{
				DBG_Put((TCHAR*)strMessage);
			}
			
			// FMOD
			*ppBuffer = pSoundBuffer->m_pSoundBuffer;
			*pDelayTime = pSoundBuffer->m_nStartTime;

			return TRUE;
		}
	}
};