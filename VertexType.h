#pragma once

namespace XiahGameEngine
{

	//---------------------------------------------------------------------------------------
	// XYZ | NORMAL | TEX2
	struct VT_Normal // 32 byte
	{
		union
		{
			struct{
				Vector3 pos;
				Vector3 normal;
				Vector2 tex;
			};
			struct{
				float xyz[ 3];
				float n_xyz[ 3];
				float tex_xy[ 2];
			};
			struct{
				float x,y,z;
				float nx,ny,nz;
				float tu,tv;
			};
			float value[ 8];
		};
	};

	#define D3DFVF_VERTEX	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

	//---------------------------------------------------------------------------------------
	// XYZ | DIFFUSE | TEX2
	struct VT_LVertex	// 24 byte
	{
		VT_LVertex() {}
		union
		{
			struct{
				Vector3 pos;
				P_COLOR diffuse;
				Vector2 tex;
			};
			struct{
				float xyz[ 3];
				unsigned char argb[ 4];
				float tex_xy[ 2];
			};
			struct{
				float x,y,z;
				unsigned char a,r,g,b;
				float tu,tv;
			};
		};
	};

	#define D3DFVF_LVERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)


	//---------------------------------------------------------------------------------------
	// XYZ | RHW | DIFFUSE | TEX2
	struct VT_TLVertex	// 28 byte
	{
		VT_TLVertex() {}
		union
		{
			struct{
				Vector4 pos;
				P_COLOR diffuse;
				Vector2 tex;
			};
/*
			struct{
				float xyzw[ 4];
				unsigned char argb[ 4];
				float tex_xy[ 2];
			};
			struct{
				float x,y,z;
				unsigned char a,r,g,b;
				float tu,tv;
			};
*/
		};
	};

	#define D3DFVF_TLVERTEX	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)


	//---------------------------------------------------------------------------------------
	// XYZB2 | BYTE4 | NORMAL | TEX2
	struct SkinVertex
	{
		float			position[ 3];		// À§Ä¡
		float			weight[ 3];			// Blend Weight
		unsigned long	indices;			// Blend Matrix Index
		float			normal[ 3];			// Normal
		float			tex[ 2];			// TextureÁÂÇ¥
	};

	#define FVF_SKINVERTEX (D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)

	// CG_2005/05/26 : ½ºÄ«ÀÌ¸Ê
	struct SKY_VERTEX
	{
		D3DXVECTOR3 p;
		FLOAT	 tu, tv;

		static const DWORD FVF;
	};

	struct PLANET_VERTEX
	{
		D3DXVECTOR3 p;
		float    rhw;
		FLOAT    tu, tv;

		static const DWORD FVF;
	};

	struct SCloud
	{
		D3DXVECTOR3 pos;
		int      kind;
	};

	struct	e3d_dif_tex1_vertex
	{
		e3d_dif_tex1_vertex() {} 
		union 
		{
			struct { float x, y, z;		DWORD color; float u, v; };
			struct { D3DXVECTOR3 p;		DWORD color; float u, v; };
		};
	};

	struct	e3d_dif_vertex
	{
		e3d_dif_vertex() {} 
		union 
		{
			struct { float x, y, z;		DWORD color; };
			struct { D3DXVECTOR3 p;		DWORD color; };
		};
	};

	#define D3DFVF_DIF_VERTEX						( D3DFVF_XYZ | D3DFVF_DIFFUSE )
	#define D3DFVF_DIF_TEX1_VERTEX					( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
	#define D3DFVF_DIF_TEX2_VERTEX					( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 )

};