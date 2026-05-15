#include "stdafx.h"
#include "XiahPak.h"
#include "DSoundDevice.h"

// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"

#include <assert.h>

#define XIAH_PAKSTRING _T("Xiah Pak모듈")
#define XIAH_PAK_VERSION (1)

#define MAX_RES	10000
#define MAX_RES_FILE_SIZE	(4 * 1024 * 1024)	// 최대 3메가


namespace XiahGameEngine
{
	namespace XiahPak
	{

		// Global 데이터
		BYTE g_TempBuffer[ MAX_RES_FILE_SIZE];

		// CRes객체
		class CRes
		{
		public:
			CRes()
			{
				m_bRealized = FALSE;
				m_pTextureHandle = NULL;
				m_pSoundHandle = NULL;
			}

			virtual ~CRes()
			{
				Release();
			}

			void Release()
			{
				if( m_pTextureHandle)
					m_pTextureHandle->Release();

				// FMOD
				if( m_pSoundHandle) FSOUND_Sample_Free(m_pSoundHandle);

				m_pTextureHandle = NULL;
				m_pSoundHandle = NULL;

				m_bRealized = FALSE;
			}

			// 파일로 부터 Resource읽어서 타입에 따라 객체를 생성한다.
			BOOL Realize(BOOL bPreserveQuality)
			{
				if( m_bRealized)
					return TRUE;

				if( m_nSize > MAX_RES_FILE_SIZE)
				{
					throw _T("단일 파일의 사이즈가 3메가를 초과합니다.");
					return FALSE;
				}

				if( SetFilePointer( m_hFileHandle, m_nOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				{
					throw _T("파일 Offset값이 잘못 되었습니다");
					return FALSE;
				}

				DWORD nReadByte;

				if( !ReadFile( m_hFileHandle, g_TempBuffer, m_nSize, &nReadByte, NULL))
				{
					throw _T("파일을 읽는데 실패하였습니다");
					return FALSE;
				}

				HRESULT hr;

				switch( m_nType)
				{
				case 1:
				case 4:
					{
						if( bPreserveQuality)
						{
//							hr = D3DXCreateTextureFromFileInMemory( g_pDirect3DDevice, g_TempBuffer, m_nSize, &m_pTextureHandle);
							D3DXIMAGE_INFO image_info;
							hr = D3DXGetImageInfoFromFileInMemory( g_TempBuffer, m_nSize, &image_info);

							if( FAILED( hr))
							{
								DBG_LogFile( _T("Get Image Info Fail %d"),m_nType);
								throw _T("Get Image Info Fail");
							}

							UINT nWidth		= image_info.Width;
							UINT nHeight	= image_info.Height;

							hr = D3DXCreateTextureFromFileInMemoryEx( g_pDirect3DDevice, g_TempBuffer, m_nSize, nWidth, nHeight, 1, 0,
								image_info.Format, D3DPOOL_DEFAULT, D3DX_FILTER_POINT , D3DX_FILTER_LINEAR , 0, NULL, NULL, &m_pTextureHandle);

							if( FAILED( hr))
								throw _T("Create Texture Fail");
						}
						else
						{
							D3DXIMAGE_INFO image_info;
							hr = D3DXGetImageInfoFromFileInMemory( g_TempBuffer, m_nSize, &image_info);

							if( FAILED( hr))
								throw _T("Get Image Info Fail");
							
							// Texture Size Qualifing
							sSize newSize = TextureRescaleByQuality( sSize( image_info.Width, image_info.Height));

							UINT nWidth		= newSize.cx;
							UINT nHeight	= newSize.cy;

							hr = D3DXCreateTextureFromFileInMemoryEx( g_pDirect3DDevice, g_TempBuffer, m_nSize, nWidth, nHeight, 1, 0,
								image_info.Format, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR , D3DX_FILTER_LINEAR , 0, NULL, NULL, &m_pTextureHandle);

							if( FAILED( hr))
								throw _T("Create Texture Fail");
						}
					}
					break;

				// SOUND
				case 2:
					// FMOD
					m_pSoundHandle = FSOUND_Sample_Load(FSOUND_UNMANAGED,(char*)g_TempBuffer, FSOUND_LOADMEMORY  | FSOUND_NORMAL | FSOUND_2D, 0, m_nSize);
					break;

				default:
					throw _T("알 수 없는 Resource Type입니다");
					return FALSE;
				}

				m_bRealized = TRUE;

				return TRUE;
			}

		public:

			int m_nID;
			int m_nType;
			int m_nSize;
			int m_nOffset;
			
			BOOL				m_bRealized;
			HANDLE				m_hFileHandle;

			IDirect3DTexture9*	m_pTextureHandle;

			// FMOD
			FSOUND_SAMPLE*		m_pSoundHandle;
		};

		// 동적으로 new,delete않하기 위해 만들었음, 사이즈 초과 할 수도 있음
		CRes g_ResData[ MAX_RES];

		typedef std::hash_map<int,CRes*>	RESINDEXLIST;
		typedef std::vector<HANDLE>			FILEHANDLELIST;
		
		RESINDEXLIST	g_FreeResList;	// 헤더만 갖고 있는 넘들
		RESINDEXLIST	g_RealResList;	// Realize된 Resource들
		FILEHANDLELIST  g_FileHandleList;

		XIAHGE_API BOOL InitializeXiahPak(LPCTSTR *pPakFileList, unsigned long count)
		{
			// Pak파일을 읽어 들인다.
			try
			{
				unsigned long i;
				unsigned long j;
				
				int nResCount = 0;

				DWORD nReadByte;

				int nReadValue[3000];

				for(i = 0; i < count; ++i)
				{
					HANDLE hFileHandle = CreateFile( pPakFileList[ i], 
													GENERIC_READ, 
													FILE_SHARE_READ, 
													NULL,
													OPEN_EXISTING, 
													FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
													NULL);

					if( hFileHandle == INVALID_HANDLE_VALUE)
						throw _T("Pak파일이 없습니다");

#ifdef _DEBUG
					DBG_Put(_T("Res %s"),  pPakFileList[ i]);
#endif // _DEBUG

					DWORD nTemp;

					// 버전 검사
					if( !ReadFile( hFileHandle, &nTemp, 4, &nReadByte, NULL))
						throw _T("Pak파일 읽기 에러");

					if( nTemp != XIAH_PAK_VERSION)
						throw _T("Pak파일 버전이 다름입니다.");

					// 헤더 읽기
					if( !ReadFile( hFileHandle, &nTemp, 4, &nReadByte, NULL))
						throw _T("Pak파일 읽기 에러");
					
					// test code
					if(!ReadFile(hFileHandle, &nReadValue, nTemp*16, &nReadByte, NULL))
						throw _T("Pak파일 읽기 에러");

					register int z = -1;
					for(j=0; j < nTemp; ++j)
					{
						if( nResCount >= MAX_RES)
							throw _T("Pak Res Header Handle갯수가 모자랍니다");

						CRes* pRes = &g_ResData[nResCount];

						pRes->m_nID = nReadValue[++z];
						pRes->m_nType = nReadValue[++z];
						pRes->m_nSize = nReadValue[++z];
						pRes->m_nOffset = nReadValue[++z];

						pRes->m_hFileHandle = hFileHandle;
						
						assert(pRes->m_bRealized == FALSE);
						assert(pRes->m_pSoundHandle == NULL);
						assert(pRes->m_pTextureHandle == NULL);

						if( g_FreeResList.find( pRes->m_nID) != g_FreeResList.end())
						{
							DBG_Put(_T("Res ID 중복 에러 : %d"), pRes->m_nID);
						}
						else
						{
#ifdef _DEBUG
						//	DBG_Put(_T("Res ID: %d"), pRes->m_nID);
#endif // _DEBUG
							g_FreeResList.insert( RESINDEXLIST::value_type( pRes->m_nID, pRes));
							++nResCount;
						}
					}

					g_FileHandleList.push_back( hFileHandle);
				}
			}
			catch(LPCTSTR strError)
			{
				//MessageBox(NULL, strError, XIAH_PAKSTRING, MB_OK | MB_ICONERROR);
				DBG_Put(_T("%s"), strError);
				return FALSE;
			}

			return TRUE;
		}
			
		XIAHGE_API BOOL UninitializeXiahPak()
		{
			ReleaseAllResource();

			for(unsigned long i = 0; i < g_FileHandleList.size(); ++i)
			{
				CloseHandle( g_FileHandleList[ i]);
			}
				
			g_FileHandleList.clear();

			return TRUE;
		}

		XIAHGE_API BOOL ReleaseAllResource()
		{
			RESINDEXLIST::iterator it = g_RealResList.begin();

			for( ; it != g_RealResList.end(); ++it)
			{
				CRes *pRes = it->second;

				pRes->Release();
			
				g_FreeResList.insert( RESINDEXLIST::value_type( it->first, it->second));
			}
			
			g_RealResList.clear();
		
			return TRUE;
		}

		CRes *GetRes(int nResID, BOOL bPreserveQuality)
		{
			RESINDEXLIST::iterator it = g_RealResList.find( nResID);

			if( it != g_RealResList.end())
				return it->second;

			it = g_FreeResList.find( nResID);

			if( it != g_FreeResList.end())
			{
				CRes *pRes = it->second;
				
				// Real옮겨 준다
				try
				{
					pRes->Realize(bPreserveQuality);
				}
				catch(LPCTSTR strError)
				{
//					MessageBox(NULL, strError, XIAH_PAKSTRING, MB_OK | MB_ICONERROR);
					DBG_Put(_T("%s"), strError);
					return NULL;
				}

				g_FreeResList.erase( it);
				g_RealResList.insert( RESINDEXLIST::value_type( pRes->m_nID, pRes));

				return pRes;
			}

			return NULL;
		}

		XIAHGE_API IDirect3DTexture9 *GetTexture(int nResID, BOOL bPreserveQuality)
		{
			CRes* pRes = GetRes(nResID, bPreserveQuality);

			if( pRes == NULL)
				return NULL;

			return pRes->m_pTextureHandle;		
		}

		// PAK에서 SOUND를 가져온다.
		// FMOD
		XIAHGE_API FSOUND_SAMPLE	*GetSound(int nResID, BOOL bPreserveQuality)
		{
			CRes* pRes = GetRes( nResID, 0);

			if( pRes == NULL)
				return NULL;

			return pRes->m_pSoundHandle;
		}

		XIAHGE_API BOOL ReleaseRes(int nResID)
		{			
			RESINDEXLIST::iterator it = g_FreeResList.find( nResID);

			if( it != g_FreeResList.end())
				return TRUE;

			it = g_RealResList.find( nResID);

			if( it == g_RealResList.end())
				return FALSE;

			CRes *pRes = it->second;
			pRes->Release();

			g_RealResList.erase( it);

			g_FreeResList.insert( RESINDEXLIST::value_type( pRes->m_nID, pRes));

			return TRUE;
		}
	};
};