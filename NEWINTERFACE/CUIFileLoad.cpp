/****************************************************************************************************
	파 일 명:   CUIFileLoad.cpp
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIFileLoad.h"
#include <io.h>

using namespace std;

namespace XiahGameEngine
{
	CUIFileLoad::CUIFileLoad() : m_pFile(NULL)
	{		
	}

	CUIFileLoad::~CUIFileLoad()
	{		
	}

	/**
	 * Cmr 로딩
	 * \param strCmrFileName 파일명
	 * \return 성공여부
	 */
	bool CUIFileLoad::CtrlLoad(const TCHAR* strCmrFileName)
	{
		FILE* fp = NULL;

		if((fp = _tfopen(strCmrFileName, _T("rb"))) != NULL)
		{
			int nControlCount	= 0;
			int nVersion		= 0;			
			int	nResID			= 0;
			byte byType			= 0;

			DWORD dwFileSize = filelength(fileno(fp));
			char *buffer = new char[dwFileSize + 1];
			char *pPreBuffer = buffer;

			fread(buffer, dwFileSize, 1, fp);
			fclose(fp);

			*(buffer + dwFileSize) = 0;

			buffer += 52;	// 필요없는 글

			memcpy(&nControlCount, buffer, 4);		buffer += 4;
			memcpy(&nVersion, buffer, 4);			buffer += 4;

			for(int i=0; i < nControlCount; ++i)
			{
				sCtrlData *pData = new sCtrlData;

				memcpy(&byType, buffer, 1);			++buffer;
				memcpy(&pData->nID, buffer, 4);		buffer += 4;
				memcpy(&pData->nAlpha, buffer, 4);	buffer += 10;

				/*
				fread(&nBorder,			sizeof(int),	1, fp);
				fread(&byBorderLength,	sizeof(byte),	1, fp);
				fread(&byBorderAlpha,	sizeof(byte),	1, fp);
				*/

				pData->nType = static_cast<int>(byType);

				// 이전 로딩 구조 X같다
				switch(byType)
				{
				case PROGRESS:
					{
						DefaultRead(pData, &buffer);
					}
					break;

				case BUTTON:
					{
						memcpy(&pData->nCount, buffer, 4);			buffer += 4;

						if(pData->nCount != 3) // 꼭 3개
						{
							DBG_LogFile( _T("UI FileLoad::CtrlLoad Button Count fail - [ count %d/3 ]"), pData->nCount);
							return false;
						} // if(pData->nCount != 3) // 꼭 3개

						pData->nCount = 0;

						for(int i=0; i < 3; ++i)
						{
							memcpy(&pData->nX, buffer, 4);			buffer += 4;
							memcpy(&pData->nY, buffer, 4);			buffer += 4;
							memcpy(&pData->nWidth, buffer, 4);		buffer += 4;
							memcpy(&pData->nHeight, buffer, 4);		buffer += 4;
							memcpy(&pData->fU, buffer, 4);			buffer += 4;
							memcpy(&pData->fV, buffer, 4);			buffer += 4;

							memcpy(&nResID, buffer, 4);				buffer += 4;

							pData->nX = static_cast<float>(pData->nX) - 0.5f;
							pData->nY = static_cast<float>(pData->nY) - 0.5f;
							pData->nWidth  = static_cast<float>(pData->nWidth) * pData->fU - 0.5f;
							pData->nHeight = static_cast<float>(pData->nHeight) * pData->fV - 0.5f;

							switch(i)
							{
							case 0:
								pData->nResID = nResID;
								break;
							case 1:
								pData->nResID2 = nResID;
								break;
							case 2:
								pData->nResID3 = nResID;
								break;					
							}
						}
					}
					break;

				case EDITBOX:
					{
						DefaultRead(pData, &buffer);
					}
					break;

				case STATICLIST:
					{
						memcpy(&pData->nCount, buffer, 4);		buffer += 4;

						for(int i=0; i < pData->nCount; ++i)
						{
							sCtrlData *pSLData = new sCtrlData;

							DefaultRead(pSLData, &buffer);
							pSLData->nID = i;

							pData->vCtrlList.push_back(pSLData);							
						}
					}
					break;

				case STATIC:
				case STATICDUMMY:
					{
						if(pData->nID >= 447 && pData->nID <= 482)
						{
							DefaultRead(pData, &buffer, true);
						}
						else
						{
							DefaultRead(pData, &buffer);
						}
					}
					break;

				case TOOLTIP:
					break;
				case COMBOBOX:
					break;

				case SCROLLBAR:
					{
						DefaultRead(pData, &buffer);
					}
					break;

				default:
					{
						DBG_LogFile( _T("UI FileLoad::CtrlLoad fail - [ ID %d / type %d ]"), pData->nID, pData->nType);
						return false;
					}
					break;
				} // switch(byType)

				m_mCtrlList.insert(std::map<int, sCtrlData*>::value_type(pData->nID, pData));
			} // for(int i=0; i < nControlCount; ++i)

			delete [] pPreBuffer, pPreBuffer = NULL;
		} // if( (fp = _tfopen( szFileName, _T("rb"))) != NULL)
		else
			return false;

		return true;
	}

	/**
	 * 기본적인 형태 읽기
	 * \param *pData 
	 * \param *fp 
	 */
	inline void CUIFileLoad::DefaultRead(sCtrlData *pData, char **buffer, bool bS)
	{
		memcpy(&pData->nX, *buffer, 4);			*buffer += 4;
		memcpy(&pData->nY, *buffer, 4);			*buffer += 4;
		memcpy(&pData->nWidth, *buffer, 4);		*buffer += 4;
		memcpy(&pData->nHeight, *buffer, 4);	*buffer += 4;
		memcpy(&pData->fU, *buffer, 4);			*buffer += 4;
		memcpy(&pData->fV, *buffer, 4);			*buffer += 4;
		memcpy(&pData->nResID, *buffer, 4);		*buffer += 4;

		if(bS)
		{
			pData->nX = static_cast<float>(pData->nX);	// - 0.5f;
			pData->nY = static_cast<float>(pData->nY);	// - 0.5f;
			pData->nWidth	= static_cast<float>(pData->nWidth) * pData->fU;	// - 0.5f;
			pData->nHeight  = static_cast<float>(pData->nHeight) * pData->fV;	// - 0.5f;
		}
		else
		{
			pData->nX = static_cast<float>(pData->nX) - 0.5f;
			pData->nY = static_cast<float>(pData->nY) - 0.5f;
			pData->nWidth	= static_cast<float>(pData->nWidth) * pData->fU - 0.5f;
			pData->nHeight  = static_cast<float>(pData->nHeight) * pData->fV - 0.5f;
		}
	}

	/**
	 * Fmr 로딩
	 * \param strFmrFileName 파일명
	 * \return 프레임 수
	 */
	int CUIFileLoad::FrameLoad(const TCHAR* strFmrFileName)
	{
		if((m_pFile = _tfopen( strFmrFileName, _T("rb"))) != NULL)
		{
			int nFrameCount =0;

			fseek(m_pFile, 50, SEEK_CUR);
			fread(&nFrameCount, sizeof(int), 1, m_pFile);
			fseek(m_pFile, 4, SEEK_CUR);

			return nFrameCount;
		}

		return -1;
	}

	/**
	 * 프레임 정보 읽기
	 * \param &data 
	 */
	void CUIFileLoad::FrameRead(sFrameData &data)
	{		
		fread(&data.nID,		sizeof(int), 1, m_pFile);
		fread(&data.nHeight,	sizeof(int), 1, m_pFile);
		fread(&data.nWidth,		sizeof(int), 1, m_pFile);
		//fread(&Color, sizeof(int), 1, fp);
		fseek(m_pFile, 4, SEEK_CUR);
		fread(&data.nAlpha,		sizeof(int), 1, m_pFile);
		fread(&data.nResID,		sizeof(int), 1, m_pFile);
		fread(&data.nCtrlCount, sizeof(int), 1, m_pFile);
	}

	/**
	 * 프레임의 컨트롤 읽기
	 * \param &data 
	 */
	void CUIFileLoad::CtrlRead(sCtrlData &data)
	{
		int nPosX =0, nPosY =0, nSeq =0;

		fread(&data.nID, sizeof(int), 1, m_pFile); // 컨트롤 ID
		fread(&nPosX,	sizeof(int), 1, m_pFile);
		fread(&nPosY,	sizeof(int), 1, m_pFile);
		fread(&nSeq,	sizeof(int), 1, m_pFile);	// 순서
		//fseek(m_pFile, 4, SEEK_CUR);
		fread(&data.nType, sizeof(int), 1, m_pFile);

		std::map<int, sCtrlData*>::iterator iter = m_mCtrlList.find(data.nID);
		if (iter == m_mCtrlList.end())
		{
			data.nID = nSeq;
			data.nAlpha = 255;
			data.nX = nPosX;
			data.nY = nPosY;
			data.nWidth = 0;
			data.nHeight = 0;
			data.fU = 0.0f;
			data.fV = 0.0f;
			data.nCount = 0;
			return;
		}
		sCtrlData *pCtrlData = iter->second;

		data.nID = nSeq; // 앞으로 컨트롤 ID는 필요 없다.

		data.nAlpha = pCtrlData->nAlpha;

		data.nX = pCtrlData->nX + nPosX;
		data.nY = pCtrlData->nY + nPosY;
		data.nWidth = pCtrlData->nWidth + nPosX;
		data.nHeight = pCtrlData->nHeight + nPosY;

		data.fU = pCtrlData->fU;
		data.fV = pCtrlData->fV;
		data.nCount = pCtrlData->nCount;

		// Ctrl Type
		switch(data.nType)
		{
		case PROGRESS:
			data.nResID = pCtrlData->nResID;
			break;
		case BUTTON:
			data.nResID = pCtrlData->nResID;
			data.nResID2 = pCtrlData->nResID2;
			data.nResID3 = pCtrlData->nResID3;
			break;
		case EDITBOX:
			data.nResID = pCtrlData->nResID;
			break;
		case STATICLIST:
			{
				std::vector<sCtrlData*>::iterator iter = pCtrlData->vCtrlList.begin();

				int nTempX = 1024;
				int nTempY = 768;
				int nTempX2 = 0;
				int nTempY2 = 0;

				for(; iter != pCtrlData->vCtrlList.end(); ++iter)
				{
					sCtrlData *pData = *iter;
					sCtrlData *pAddData = new sCtrlData;

					pAddData->nX = pData->nX + nPosX;
					pAddData->nY = pData->nY + nPosY;
					pAddData->nWidth  = pData->nWidth  + nPosX;
					pAddData->nHeight = pData->nHeight + nPosY;

					pAddData->nID = pData->nID;
					pAddData->fU = pData->fU;
					pAddData->fV = pData->fV;
					pAddData->nResID = pData->nResID;

					//////////////////////////////////////////////////////////////////////////
					if(nTempX > pAddData->nX)
						nTempX = pAddData->nX;
					if(nTempX2 < pAddData->nWidth)
						nTempX2 = pAddData->nWidth;
					if(nTempY > pAddData->nY)
						nTempY = pAddData->nY;
					if(nTempY2 < pAddData->nHeight)
						nTempY2 = pAddData->nHeight;
					//////////////////////////////////////////////////////////////////////////										

					data.vCtrlList.push_back(pAddData);
				} // for(; iter != pCtrlData->vCtrlList.end(); iter++)

				data.nX = nTempX;
				data.nY = nTempY;
				data.nWidth = nTempX2;
				data.nHeight = nTempY2;
			}
			break;
		case STATIC:
		case STATICDUMMY:
			data.nResID = pCtrlData->nResID;
			break;

		case COMBOBOX:
			break;
		case SCROLLBAR:
			{
				data.nResID = pCtrlData->nResID;
			}
			break;
		default:
			DBG_LogFile( _T("UI FileLoad::CtrlRead fail - [ ID %d / type %d ]"), pCtrlData->nID, pCtrlData->nType);
			break;
		}
	}

	/**
	 * 파일닫기
	 */
	void CUIFileLoad::FrameEnd()
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	/**
	 * 컨트롤 해제
	 */
	void CUIFileLoad::CtrlRelease()
	{
		std::map<int, sCtrlData*>::iterator iter = m_mCtrlList.begin();

		for( ; iter != m_mCtrlList.end(); ++iter)
		{
			sCtrlData *pControl = iter->second;

			if(pControl->nCount)
			{
				std::vector<sCtrlData*>::iterator iterlist = pControl->vCtrlList.begin();

				for(; iterlist != pControl->vCtrlList.end(); ++iterlist)
				{
					sCtrlData *pCtrlTemp = *iterlist;

					delete pCtrlTemp;
					pCtrlTemp = NULL;					
				}

				pControl->vCtrlList.clear();
			} // if(pControl->nCount)

			delete pControl;
			pControl = NULL;
		}

		m_mCtrlList.clear();
	}
};