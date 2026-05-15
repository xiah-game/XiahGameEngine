#pragma once

namespace XiahGameEngine
{
	namespace Map
	{
		// Map의 Terrain에 밀착된 사각형객체 이다
		// 위치와 Texture를 세팅해주면 그려준다
		#define MAX_DECAL_SIZE	16	// 시발 1 MapCell짜리는 없겠지? 만들지 마라~~ 응?
		
		class CMapDecal : public CRenderObject
		{
		public:
			XIAHGE_API CMapDecal();
			XIAHGE_API virtual ~CMapDecal();

			//YS_0728 : BUGFIX
			XIAHGE_API void Release ();

			// 일단 정사각형만 지원하겠다
			XIAHGE_API BOOL Create(IDirect3DTexture9* pTexture,float x,float y,float size,D3DCOLOR color = D3DCOLOR_XRGB( 255, 255, 255), BOOL z=FALSE, BYTE byType=0, BOOL bCheckMapObject=FALSE,float fPosHeight=0.0f,float fCharHeight=0.0f,float fCharPositionY=0.0f);
			XIAHGE_API BOOL Move(float x,float y);
		
			XIAHGE_API BOOL BuildDecal();
			XIAHGE_API BOOL Render();

			XIAHGE_API inline BOOL SetSize(float size)
			{
				if(size > 1.0f || size < 0)
					return FALSE;

				m_AlPhaValue = 255 - (240 * size);

				return TRUE;
			}

			XIAHGE_API inline BOOL SetRotate( float speed)
			{
				m_RotateSpeed = speed;

				return TRUE;
			}

			XIAHGE_API inline BOOL SetDecalZ(BOOL z)
			{
				m_bCheckZ = z;
				return TRUE;
			}

			XIAHGE_API inline void SetCheckMapObject(BOOL bValue)
			{
				m_bCheckMapObject = bValue;
			}

		protected:
			VT_LVertex		m_Vertex[ MAX_DECAL_SIZE * MAX_DECAL_SIZE * 4];
			int				m_nVertex;

			unsigned short	m_Face[ MAX_DECAL_SIZE * MAX_DECAL_SIZE * 6];
			int				m_nFace;

			Vector2			m_Pos;
			float			m_Size;
			D3DCOLOR		m_Color;

			float			m_RotateSpeed;
			float			m_RotateAngle;
			BOOL			m_bCheckZ;

			IDirect3DTexture9*	m_pTexture;
			long	m_AlPhaValue;

			BOOL			m_bCheckMapObject;	// 맵 오브젝트위에 있는지 체크.

			float			m_fPosHeightForMouseCursor;
			float			m_fMainCharHeight;
			float			m_fMainCharPositionY;

			// 현재 이 녀석이 그림자인지, 단 비무 표시를 위한 특별한 그림자 인지 구분.
			BYTE		m_byRenderType;

		public:
			BOOL			m_bOnMapObject;		// 맵 오브젝트위에 존재한다.

		};
	};
};