// HCHFile.h: interface for the CHCHFile class.
//
//////////////////////////////////////////////////////////////////////

#include "EffectCommonDef.h"

///////////////////////////  Set Structures

namespace XiahGameEngine
{

typedef struct tagFACEDATAm
{
	short			nIndex[3];
} FACEDATAm, *LPFACEDATAm;

typedef struct tagCOLORVALUEm
{
	float		fRed;
	float		fGreen;
	float		fBlue;
} COLORVALUEm, *LPCOLORVALUEm;

typedef struct tagMATERIALm
{
	COLORVALUEm		clrAmbient;
	COLORVALUEm		clrDiffuse;
} MATERIALm, *LPMATERIALm;

typedef struct tagD3DVERTEXm
{
	D3DVECTOR vPosition;
	D3DVECTOR vNormal;
	float tu, tv;
} D3DVERTEXm, *LPD3DVERTEXm;

typedef struct tagFACEBUFFERm
{
	LPFACEDATAm		pFace;
	int				nCount;
} FACEBUFFERm, *LPFACEBUFFERm;

typedef struct tagMESHBUFFERm
{
	int				nVertexCount;
	int				nFaceCount;

	LPD3DVERTEXm		pVertex;
	LPFACEDATAm		pFace;
} MESHBUFFERm, *LPMESHBUFFERm;

typedef struct tagHCHFILE
{
	int				nMeshCount;
	int				nFrameCount;
	int				nBoneCount;

	LPMATERIALm		pMaterial;
	D3DMATRIX*		pBone;
	LPMESHBUFFERm	pMesh;
	LPFACEBUFFERm	pFaceBuffer;

	BOOL*			pDrawBone;
	MyString		strFileName;
} HCHFILE, *LPHCHFILE;


////////////////////////// Class

class CHCHFile2  
{
	D3DMATRIX	m_Position;
	bool		m_bLoaded;
	int			m_nCurFrame;

public:
	HCHFILE		m_HCHFile;

	bool IsLoaded();
	void Render(LPDIRECT3DDEVICE9 pd3dDevice);
	void DeleteHCH();
	void InitHCH();
	void SetMeshCount();
	void LoadFile( LPCTSTR strFileName );
	CHCHFile2();
	virtual ~CHCHFile2();

};



};// namespace

