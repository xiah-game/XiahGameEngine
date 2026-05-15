// HCHFile.cpp: implementation of the CHCHFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HCHFile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace XiahGameEngine
{

CHCHFile2::CHCHFile2()
{
	InitHCH();
}

CHCHFile2::~CHCHFile2()
{
	DeleteHCH();
}

void CHCHFile2::LoadFile(LPCTSTR strFileName)
{
	FILE *file = _tfopen( strFileName, _T("rb") );
	if( !file ) return;
	DeleteHCH();

	fread( &m_HCHFile.nMeshCount, sizeof(int), 1, file );
	SetMeshCount();

	fread( &m_HCHFile.nFrameCount, sizeof(int), 1, file );
	fread( &m_HCHFile.nBoneCount, sizeof(int), 1, file );

	m_HCHFile.pBone = new D3DMATRIX[m_HCHFile.nBoneCount * m_HCHFile.nFrameCount];
	m_HCHFile.pDrawBone = new BOOL[m_HCHFile.nBoneCount];
	m_HCHFile.pMesh = new MESHBUFFERm[m_HCHFile.nMeshCount];

	fread( m_HCHFile.pMaterial, sizeof(MATERIALm), m_HCHFile.nMeshCount, file );
	fread( m_HCHFile.pBone, sizeof(D3DMATRIX), m_HCHFile.nBoneCount * m_HCHFile.nFrameCount, file );

	for(int i=0; i<m_HCHFile.nMeshCount; i++)
	{
		fread( &m_HCHFile.pMesh[i].nVertexCount, sizeof(int), 1, file );
		fread( &m_HCHFile.pMesh[i].nFaceCount, sizeof(int), 1, file );

		m_HCHFile.pMesh[i].pVertex = new D3DVERTEXm[m_HCHFile.pMesh[i].nVertexCount * m_HCHFile.nFrameCount];
		m_HCHFile.pMesh[i].pFace = new FACEDATAm[m_HCHFile.pMesh[i].nFaceCount];

		fread( m_HCHFile.pMesh[i].pFace, sizeof(FACEDATAm), m_HCHFile.pMesh[i].nFaceCount, file );
		fread( m_HCHFile.pMesh[i].pVertex, sizeof(D3DVERTEXm), m_HCHFile.pMesh[i].nVertexCount * m_HCHFile.nFrameCount, file );
	}// for()

	for(i=0; i<m_HCHFile.nBoneCount; i++) m_HCHFile.pDrawBone[i] = FALSE;
	m_HCHFile.strFileName = strFileName;
	m_bLoaded = true;

	fclose(file);
}

void CHCHFile2::SetMeshCount()
{
	m_HCHFile.pFaceBuffer = new FACEBUFFERm[m_HCHFile.nMeshCount];
	m_HCHFile.pMaterial = new MATERIALm[m_HCHFile.nMeshCount];

	for(int i=0; i<m_HCHFile.nMeshCount; i++)
	{
		m_HCHFile.pMaterial[i].clrAmbient.fRed = 0.0f;
		m_HCHFile.pMaterial[i].clrAmbient.fGreen = 0.0f;
		m_HCHFile.pMaterial[i].clrAmbient.fBlue = 0.0f;
		m_HCHFile.pMaterial[i].clrDiffuse.fRed = 0.0f;
		m_HCHFile.pMaterial[i].clrDiffuse.fGreen = 0.0f;
		m_HCHFile.pMaterial[i].clrDiffuse.fBlue = 0.0f;

		m_HCHFile.pFaceBuffer[i].nCount= 0;
		m_HCHFile.pFaceBuffer[i].pFace = NULL;
	}// for
}

void CHCHFile2::InitHCH()
{
	memset( &m_Position, 0, sizeof(m_Position) );
	m_Position._11 = m_Position._22 = m_Position._33 = m_Position._44 = 1;

	m_HCHFile.nMeshCount = 0;
	m_HCHFile.nFrameCount = 0;
	m_HCHFile.nBoneCount = 0;

	m_HCHFile.pMaterial = NULL;
	m_HCHFile.pBone = NULL;
	m_HCHFile.pMesh = NULL;
	m_HCHFile.pFaceBuffer = NULL;

	m_HCHFile.pDrawBone = NULL;
	m_HCHFile.strFileName = _T("");

	m_bLoaded = false;
	m_nCurFrame = 0;
}

void CHCHFile2::DeleteHCH()
{
	if( m_HCHFile.pFaceBuffer )
	{
		for(int i=0; i<m_HCHFile.nMeshCount; i++)
			if( m_HCHFile.pFaceBuffer[i].pFace )
				delete []m_HCHFile.pFaceBuffer[i].pFace;

		delete []m_HCHFile.pFaceBuffer;
	}

	if( m_HCHFile.pMaterial ) delete []m_HCHFile.pMaterial;
	if( m_HCHFile.pBone ) delete []m_HCHFile.pBone;
	if( m_HCHFile.pDrawBone ) delete []m_HCHFile.pDrawBone;

	if( m_HCHFile.pMesh )
	{
		for(int i=0; i<m_HCHFile.nMeshCount; i++)
		{
			if( m_HCHFile.pMesh[i].pVertex )
			{
				delete []m_HCHFile.pMesh[i].pVertex;
				m_HCHFile.pMesh[i].pVertex = NULL;
			}

			if( m_HCHFile.pMesh[i].pFace )
			{
				delete []m_HCHFile.pMesh[i].pFace;
				m_HCHFile.pMesh[i].pFace = NULL;
			}
		}// for

		delete []m_HCHFile.pMesh;
	}

	InitHCH();
}

void CHCHFile2::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr;
	for(int i=0; i<m_HCHFile.nMeshCount; i++) // for each mesh
	{
		int nVertex		= m_HCHFile.pMesh[i].nVertexCount;
		int nFaces		= m_HCHFile.pMesh[i].nFaceCount;
		WORD *Index = new WORD[3*nFaces+1];
		memset( Index, 0, sizeof(Index) );

		int nIndexIndex = 0;
		for(int j=0; j<nFaces; j++)	// copy index data of face
		{
			Index[nIndexIndex++] = m_HCHFile.pMesh[i].pFace[j].nIndex[0];
			Index[nIndexIndex++] = m_HCHFile.pMesh[i].pFace[j].nIndex[1];
			Index[nIndexIndex++] = m_HCHFile.pMesh[i].pFace[j].nIndex[2];
		}

		g_Device.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
		//pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
		hr = pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, nVertex, nFaces, Index, D3DFMT_INDEX16, 
											m_HCHFile.pMesh[i].pVertex + m_nCurFrame * m_HCHFile.pMesh[i].nVertexCount, sizeof(D3DVERTEXm) );

		delete []Index;
	}// for(i)

	m_nCurFrame = (++m_nCurFrame) % m_HCHFile.nFrameCount;
}

bool CHCHFile2::IsLoaded()
{
	return m_bLoaded;
}


}; // namespace
