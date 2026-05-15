#include "stdafx.h"
#include "MapData.h"
#include "MapDecal.h"
#include "MapRender.h"
#include "XiahGameEngineBase.h"

#include "XiahPak.h"

namespace XiahGameEngine
{
	namespace Map
	{
		XIAHGE_API CMapDecal::CMapDecal()
		{
			m_nVertex = 0;
			m_nFace = 0;
			m_RotateSpeed = 0;
			m_RotateAngle = 0;
			m_bCheckZ = FALSE;
			m_AlPhaValue = 0;
			m_bCheckMapObject = FALSE;
			m_bOnMapObject = FALSE;
		}

		XIAHGE_API CMapDecal::~CMapDecal()
		{
		}

//////////////////////////////////////////////////////////////////////////
// 약간 띄워 준다 ㅡ,,ㅡ
#define SETVERTEX( index, x_pos, y_pos) \
		vx = (j + tx + x_pos) * TILE_GRID_SIZE;\
		vy = (-i + ty + y_pos) * TILE_GRID_SIZE;\
		height[ index] = g_MapRes.GetHeight( vx, vy);\
		pVertex[ index].pos = Vector3(  vx, height[ index] + 0.110f, vy);\
		pVertex[ index].tu = (vx - decal_min.x) / m_Size;\
		pVertex[ index].tv = -(vy - decal_min.y) / m_Size;\
		pVertex[ index].diffuse = m_Color;
//////////////////////////////////////////////////////////////////////////
		//YS_0728 : BUGFIX
		XIAHGE_API void CMapDecal::Release ()
		{
			m_nVertex			= 0;
			m_nFace				= 0;
			m_RotateSpeed		= 0;
			m_RotateAngle		= 0;
			m_bCheckZ			= FALSE;
			m_AlPhaValue		= 0;
			m_bCheckMapObject	= FALSE;
			m_bOnMapObject		= FALSE;			
		}

		XIAHGE_API BOOL CMapDecal::BuildDecal()
		{
			if( m_Size / TILE_GRID_SIZE > MAX_DECAL_SIZE)
				m_Size = MAX_DECAL_SIZE * TILE_GRID_SIZE;

			int tx,ty;
			int tcount;
			int i,j;
			float vx,vy;

			tx = (m_Pos.x - m_Size / 2) / TILE_GRID_SIZE ;
			ty = (m_Pos.y + m_Size / 2) / TILE_GRID_SIZE - 1;

			tcount = m_Size / TILE_GRID_SIZE + 2;

			Vector2 decal_min, decal_max;

			// 데칼의 실제 범위
			decal_min = Vector2( m_Pos.x - m_Size / 2, m_Pos.y + m_Size / 2);
			decal_max = Vector2( m_Pos.x + m_Size / 2, m_Pos.y - m_Size / 2);

			// 마우스 커서가 맵 오브젝트 위에 있는지 검사한다.
			// 마우스 위치가 아닌 범위로 검사한다. 이게 더 정확하다.
			if( m_bCheckMapObject )
			{
				// 클릭한 마우스 위치.
//				Vector3 vMousePos = Vector3( m_Pos.x, m_fPosHeightForMouseCursor, m_Pos.y );

				BBoxAABB3 MouseBound = BBoxAABB3( Vector3( decal_min.x, m_fPosHeightForMouseCursor, decal_max.y ),
												  Vector3( decal_max.x, m_fPosHeightForMouseCursor, decal_min.y ) );

				// 마우스 위치 커서가 맵 오브젝트위에 있는지 검사, 캐릭터가 있는 위치까지 보낸다.
				m_bOnMapObject = g_MapRender.CheckMapObjectForMouseCursor( MouseBound, m_fMainCharHeight, m_fMainCharPositionY );
			}

			//
			m_nVertex = 0;
			m_nFace = 0;
			VT_LVertex *pVertex = &m_Vertex[ 0];
#ifdef TRACE_LOG
			if(pVertex == NULL)
			{
				DBG_LogFile( _T("CMapDecal::BuildDecal fail"));
			}
#endif
			unsigned short *pFace = &m_Face[ 0];
			if(pFace == NULL)
			{
				DBG_LogFile( _T("CMapDecal::BuildDecal2 fail"));
			}

			if( !m_bOnMapObject )
			{
				// vx = ( 0 + tx + 0 ) * TILE_GRID_SIZE
				// vy = (-0 + ty + 1 ) * TILE_GRID_SIZE
				// 를 시작 위치로 가로, 세로 길이가 4인 4개의 버텍스를
				// 가로 세로 tcount개 씩 총 tcount*tcount 개를 만든다.
				for(i = 0; i < tcount; i++)	// y
				{
					for(j = 0; j < tcount; j++) // x
					{
						float height[ 4];

						SETVERTEX( 0, 0, 0);
						SETVERTEX( 1, 1, 0);
						SETVERTEX( 2, 0, 1);
						SETVERTEX( 3, 1, 1);

						pFace[ 0] = m_nVertex + 0;
						pFace[ 1] = m_nVertex + 2;
						pFace[ 2] = m_nVertex + 1;
						pFace[ 3] = m_nVertex + 2;
						pFace[ 4] = m_nVertex + 3;
						pFace[ 5] = m_nVertex + 1;

						pVertex += 4;
						m_nVertex += 4;

						pFace += 6;
						m_nFace += 6;
					}// for
				}// for
			}// if
			else	// 맵 오브젝트 위에 있다.
			{
				// 데칼의 실제 위치.
				Vector2 vSmall = Vector2( decal_min.x, decal_max.y );
				Vector2 vBig   = Vector2( decal_max.x, decal_min.y );

				// 이 함수에서 맵 셀과 맵 오브젝트를 찾아서 알아서 버텍스를 만들어 준다. 복잡한 함수.
				g_MapRender.MakeMouseCursorVertexOnObject( vSmall, vBig, m_Color, pVertex, pFace, m_nVertex, m_nFace );

			}// if
		
			return TRUE;
		}

		XIAHGE_API BOOL CMapDecal::Create(IDirect3DTexture9* pTexture,float x,float y,float size,D3DCOLOR color,BOOL z, BYTE byType, BOOL bCheckMapObject,float fPosHeight,float fCharHeight,float fCharPositionY)
		{
			// 똑같은것을 매번 할 필요가 없겠자.
			if( m_Pos == Vector2( x, y) && m_Size == size && m_byRenderType == byType )
				return TRUE;

			m_Pos = Vector2( x, y);
			m_Size = size;
			m_pTexture = pTexture;
			m_Color = color;
			m_bCheckZ = z;

			m_bCheckMapObject = bCheckMapObject;
			m_fPosHeightForMouseCursor = fPosHeight;
			m_fMainCharHeight = fCharHeight;
			m_fMainCharPositionY = fCharPositionY;

			m_bOnMapObject = FALSE;

			// 일반 그림자, 단 비무에서 나오는 특별한 그림자를 구분
			// 0 : 일반 그림자.
			// 1 : 단 비무에서 단주
			// 2 : 단 비무에서 단원
			// 3 : 문주
			m_byRenderType = byType;

			if( m_byRenderType == 1 || m_byRenderType == 3 )
                SetRotate( _PI / 90.0f );

			BuildDecal();

			return TRUE;
		}

		XIAHGE_API BOOL CMapDecal::Move(float x,float y)
		{
			Vector2 pos( x, y);

			if( m_Pos != pos)
			{
				m_Pos = pos;
				BuildDecal();
			}

			return TRUE;
		} 
	
		/*
		XIAHGE_API BOOL CMapDecal::SetRotate( float speed)
		{
			m_RotateSpeed = speed;
			//m_RotateAngle = 0;
			return TRUE;
		}
		*/

		/*
		XIAHGE_API BOOL CMapDecal::SetSize(float size)
		{
			if(size > 1.0f || size < 0) return FALSE;

			m_AlPhaValue = 255 - (240 * size);

			return TRUE;
		}
		*/

		XIAHGE_API BOOL CMapDecal::Render()
		{
			if( m_nVertex == 0 || m_nFace == 0)
				return FALSE;

			Matrix4x4 iTM;

			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
			//g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);

			if( m_RotateSpeed != 0)
			{
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

				Matrix4x4 rotM;
				
				m_RotateAngle += m_RotateSpeed * g_fFrameScale;

				if( m_RotateAngle > 2 * _PI)
					m_RotateAngle -= 2 * _PI;

				if( m_RotateAngle < 0)
					m_RotateAngle += 2 * _PI;
				
				rotM._11 = cos( m_RotateAngle);
				rotM._12 = sin( m_RotateAngle);
				rotM._21 = -rotM._12;
				rotM._22 = rotM._11;
				rotM._31 = (-0.5f) * rotM._11 + (-0.5f) * rotM._21 + 0.5f;
				rotM._32 = (-0.5f) * rotM._12 + (-0.5f) * rotM._22 + 0.5f;

				g_pDirect3DDevice->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX*)&rotM);
			}

			// Stage backup
			DWORD a1, a2, a3;
			DWORD b1, b2, b3;

			g_pDirect3DDevice->GetTextureStageState(0,D3DTSS_COLOROP,   &a1);
			g_pDirect3DDevice->GetTextureStageState(0,D3DTSS_COLORARG1, &a2);
			g_pDirect3DDevice->GetTextureStageState(0,D3DTSS_COLORARG2, &a3);

			g_pDirect3DDevice->GetTextureStageState(0,D3DTSS_ALPHAOP,   &b1);
			g_pDirect3DDevice->GetTextureStageState(0,D3DTSS_ALPHAARG1, &b2);
			g_pDirect3DDevice->GetTextureStageState(0,D3DTSS_ALPHAARG2, &b3);

			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			if( m_byRenderType == 0 || m_byRenderType == 3)	// 일반 그림자
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_AlPhaValue, m_AlPhaValue, m_AlPhaValue, m_AlPhaValue));
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SUBTRACT);
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			}
			else	// 단 비무 그림자.
			{
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			}

			g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);

			// Decal의 Z-buffer 활성화
			if(m_bCheckZ)
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE);
			}
			else
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE);
			}

			g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		
			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

			g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&iTM);
			g_Device.SetTexture(0, m_pTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pTexture);
			g_Device.SetStreamSource( NULL, 0);
			g_Device.SetFVF(D3DFVF_LVERTEX);
			//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX);
			g_pDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, m_nVertex, m_nFace / 3, m_Face, D3DFMT_INDEX16, m_Vertex, sizeof( VT_LVertex));

			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
			g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);

			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   a1);
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, a2);
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, a3);

			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   b1);
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, b2);
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, b3);

			if( m_RotateSpeed != 0)
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

			return TRUE;
		}
	};
};