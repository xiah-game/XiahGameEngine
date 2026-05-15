// sphere3 inline함수

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Sphere3::Sphere3()
{
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Sphere3::Sphere3(const XiahGameEngine::Vector3 &vOrigin,float fRadius)
{
	m_vOrigin = vOrigin;
	m_fRadius = fRadius;
}

//---------------------------------------------------------------------------------------
inline bool XiahGameEngine::Sphere3::Intersect(const XiahGameEngine::BBoxAABB3 &BBox) const
{
	return BBox.Intersect( *this);
}

//---------------------------------------------------------------------------------------
inline bool XiahGameEngine::Sphere3::Intersect(const XiahGameEngine::Sphere3 &sphere) const
{
	Vector3 vDistance( m_vOrigin - sphere.m_vOrigin);
	float fDistance = vDistance.GetLengthSqr();

	float fRadius = (m_fRadius + sphere.m_fRadius);
	
	if( fDistance <= (fRadius * fRadius)) return true;

	return false;
}

//---------------------------------------------------------------------------------------
// BBoxAABB Inline함수
inline XiahGameEngine::BBoxAABB3::BBoxAABB3()
{

}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::BBoxAABB3::BBoxAABB3(XiahGameEngine::Vector3 vMin,XiahGameEngine::Vector3 vMax)
{
	m_vMin = vMin;
	m_vMax = vMax;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::BBoxAABB3::Center()
{
	return (m_vMax + m_vMin) / 2;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::BBoxAABB3::Size()
{
	return (m_vMax - m_vMin);
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::BBoxAABB3 XiahGameEngine::BBoxAABB3::operator + (const XiahGameEngine::BBoxAABB3 &bbox)
{
	return BBoxAABB3( m_vMin.Min( bbox.m_vMin),
					  m_vMax.Max( bbox.m_vMax));
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::BBoxAABB3::operator += (const XiahGameEngine::BBoxAABB3 &bbox)
{
	m_vMin = m_vMin.Min( bbox.m_vMin);
	m_vMax = m_vMax.Max( bbox.m_vMax);
}

//---------------------------------------------------------------------------------------
inline bool XiahGameEngine::BBoxAABB3::Intersect(const XiahGameEngine::BBoxAABB3 &bbox) const
{
	if( m_vMin.x > bbox.m_vMax.x ||
		m_vMin.y > bbox.m_vMax.y ||
		m_vMin.z > bbox.m_vMax.z ||

		m_vMax.x < bbox.m_vMin.x ||
		m_vMax.y < bbox.m_vMin.y ||
		m_vMax.z < bbox.m_vMin.z)
		return false;

	return true;
}

//---------------------------------------------------------------------------------------
inline bool XiahGameEngine::BBoxAABB3::Intersect(const XiahGameEngine::Sphere3 &sphere) const
{
	float fDistance = 0.0f;
	for(int iAxis = 0; iAxis < 3; iAxis ++)
	{
		if( sphere.m_vOrigin[ iAxis] < m_vMin[ iAxis])
		{
			float fValue = (sphere.m_vOrigin[ iAxis] - m_vMin[ iAxis]);
			fDistance += (fValue * fValue);
		}
		else if( sphere.m_vOrigin[ iAxis] < m_vMax[ iAxis])
		{
			float fValue = (sphere.m_vOrigin[ iAxis] - m_vMax[ iAxis]);
			fDistance += (fValue * fValue);
		}
	}

	if( fDistance <= (sphere.m_fRadius * sphere.m_fRadius)) return true;
	
	return false;
}

namespace XiahGameEngine
{

//---------------------------------------------------------------------------------------
inline BBoxOBB3::BBoxOBB3()
{
	ZeroMemory( this, sizeof( BBoxOBB3));
}

//---------------------------------------------------------------------------------------
inline BBoxOBB3::BBoxOBB3(const Vector3& pos,const Vector3& size,const Matrix4x4& orient)
{
	m_Pos		= pos;
	m_Size		= size;
	m_Orient	= orient;

	BuildInternalData();
}

//---------------------------------------------------------------------------------------
inline BBoxOBB3::BBoxOBB3(const BBoxOBB3& box)
{
	CopyMemory( this, &box, sizeof( BBoxOBB3));
}

//---------------------------------------------------------------------------------------
inline BBoxOBB3::BBoxOBB3(BBoxAABB3& box,Matrix4x4& orient)
{
	m_Pos = box.Center();
	m_Size = box.Size() / 2;
	m_Orient = orient;

	BuildInternalData();
}

//---------------------------------------------------------------------------------------
inline void BBoxOBB3::SetPosition(const Vector3 &pos)
{
	m_Pos = pos;
	BuildInternalData();
}

//---------------------------------------------------------------------------------------
inline void BBoxOBB3::SetSize(const Vector3 &size)
{
	m_Size = size;
	BuildInternalData();
}

//---------------------------------------------------------------------------------------
inline void BBoxOBB3::SetOrient(const Matrix4x4 &orient)
{
	m_Orient = orient;
	BuildInternalData();
}

//---------------------------------------------------------------------------------------
inline void BBoxOBB3::BuildInternalData()
{
	int i;

//	Vector3 point[ 8];
	m_vCenter[0] = Vector3(m_Pos.x-m_Size.x, m_Pos.y, m_Pos.z);
	m_vCenter[1] = Vector3(m_Pos.x+m_Size.x, m_Pos.y, m_Pos.z);
	m_vCenter[2] = Vector3(m_Pos.x, m_Pos.y-m_Size.y, m_Pos.z);
	m_vCenter[3] = Vector3(m_Pos.x, m_Pos.y+m_Size.y, m_Pos.z);
	m_vCenter[4] = Vector3(m_Pos.x, m_Pos.y, m_Pos.z-m_Size.z);
	m_vCenter[5] = Vector3(m_Pos.x, m_Pos.y, m_Pos.z+m_Size.z);

	m_Point[0] = Vector3( m_Pos.x-m_Size.x, m_Pos.y+m_Size.y, m_Pos.z-m_Size.z );
	m_Point[1] = Vector3( m_Pos.x-m_Size.x, m_Pos.y+m_Size.y, m_Pos.z+m_Size.z );
	m_Point[2] = Vector3( m_Pos.x+m_Size.x, m_Pos.y+m_Size.y, m_Pos.z+m_Size.z );
	m_Point[3] = Vector3( m_Pos.x+m_Size.x, m_Pos.y+m_Size.y, m_Pos.z-m_Size.z );
	m_Point[4] = Vector3( m_Pos.x-m_Size.x, m_Pos.y-m_Size.y, m_Pos.z-m_Size.z );
	m_Point[5] = Vector3( m_Pos.x-m_Size.x, m_Pos.y-m_Size.y, m_Pos.z+m_Size.z );
	m_Point[6] = Vector3( m_Pos.x+m_Size.x, m_Pos.y-m_Size.y, m_Pos.z+m_Size.z );
	m_Point[7] = Vector3( m_Pos.x+m_Size.x, m_Pos.y-m_Size.y, m_Pos.z-m_Size.z );

	static short index[36] = {0,1,3, 3,1,2, 0,4,1, 1,4,5,
					 1,5,2, 2,5,6, 2,6,3, 3,6,7,
					 3,7,0, 0,7,4, 4,7,5, 5,7,6};

	for(i = 0; i < 8; i++)
	{
		m_Point[ i] *= m_Orient;
	}

	for(i = 0; i < 6; i++)
	{
		m_vCenter[ i] *= m_Orient;

		int face_index;
		
		face_index = (i * 2 + 0) * 3;
		
		m_Face[ i * 2] = Face3( m_Point[ index[ face_index + 2]],
							m_Point[ index[ face_index + 1]],
							m_Point[ index[ face_index + 0]]);
		
		face_index = (i * 2 + 1) * 3;

		m_Face[ i * 2 + 1] = Face3( m_Point[ index[ face_index + 2]],
							m_Point[ index[ face_index + 1]],
							m_Point[ index[ face_index + 0]]);

		m_Plane[ i] = m_Face[ i * 2].plane;
	}

	m_BBoxAABB.m_vMin = Vector3( 100000.0f, 100000.0f, 100000.0f);
	m_BBoxAABB.m_vMax = -m_BBoxAABB.m_vMin;
	
	m_Center = Vector3();
	for(i = 0; i < 8; i++)
	{
		if(i == 0 || m_HeightMax.y < m_Point[ i].y)
		{
			m_HeightMax = m_Point[ i];
		}

		if( m_Point[ i].x < m_BBoxAABB.m_vMin.x) m_BBoxAABB.m_vMin.x = m_Point[ i].x;
		if( m_Point[ i].y < m_BBoxAABB.m_vMin.y) m_BBoxAABB.m_vMin.y = m_Point[ i].y;
		if( m_Point[ i].z < m_BBoxAABB.m_vMin.z) m_BBoxAABB.m_vMin.z = m_Point[ i].z;

		if( m_Point[ i].x > m_BBoxAABB.m_vMax.x) m_BBoxAABB.m_vMax.x = m_Point[ i].x;
		if( m_Point[ i].y > m_BBoxAABB.m_vMax.y) m_BBoxAABB.m_vMax.y = m_Point[ i].y;
		if( m_Point[ i].z > m_BBoxAABB.m_vMax.z) m_BBoxAABB.m_vMax.z = m_Point[ i].z;

		m_Center += m_Point[ i];
	}

	m_Center /= 8;
}

//---------------------------------------------------------------------------------------
inline Vector3 BBoxOBB3::GetHeightMax()
{
	return m_HeightMax;
}

//---------------------------------------------------------------------------------------
inline bool BBoxOBB3::IsIntersect(const Vector3& start,const Vector3& end,Vector3* pStartCollidePoint)
{
	int i;
	
	if( pStartCollidePoint == NULL)
	{
		for(i = 0; i < 12; i++)
		{
			if( m_Face[ i].IsIntersect( start, end))
				return true;
		}
	}
	else
	{

		bool bCollide = FALSE;
		float fDistance = 1000.0f;
		Vector3 vCollide;
		Vector3 vDelta;

		for(i = 0; i < 12; i++)
		{
			if( m_Face[ i].IsIntersect( start, end))
			{
				vCollide = m_Face[ i].GetIntersectPoint( start, end);

				vDelta = start - vCollide;

				float fLength = vDelta.GetLength();

				if( fLength < fDistance)
				{
					fDistance = fLength;
					*pStartCollidePoint = vCollide;
				}

				bCollide = true;
			}
		}

		return bCollide;
	}

	return false;
}

//---------------------------------------------------------------------------------------
// 난중에 좀더 정확한걸로 바꿔주어야 겠다
inline bool BBoxOBB3::IsIntersect( BBoxAABB3* pAABB)
{
	int i;
	for(i = 0; i < 6; i++)
	{
		if( !m_Plane[ i].IsFront( pAABB->Center()))
			return false;
	}

	return true;
}

inline bool BBoxOBB3::IsIntersect( BBoxOBB3* pOBB)
{
	// 속도고 지랄이고 일단 정확하게
	int i;

	static int index[ 24] = {
		// 아래 뚜껑
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		// 윗 뚜껑
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		// 모서리
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	for(i = 0; i <12; i++)
	{
		if( IsIntersect( pOBB->m_Point[ index[ i * 2 + 0]], pOBB->m_Point[ index[ i * 2 + 1]], NULL))
			return true;
	}


/*
	int i;
	int j;

	for(i = 0; i < 6; i++)
	{
		BOOL bCollide = TRUE;

		for(j = 0; j < 6; j++)
		{
			if( !m_Plane[ j].IsFront( pOBB->m_vCenter[ i]))
			{
				bCollide = FALSE;
				break;
			}
		}

		if( bCollide)
			return true;
	}

	for(i = 0; i < 6; i++)
	{
		BOOL bCollide = TRUE;

		for(j = 0; j < 6; j++)
		{
//			if( !m_Plane[ j].IsFront( pOBB->m_vCenter[ i]))
			if( pOBB->m_Plane[ j].IsFront( m_vCenter[ i]))
			{
				bCollide = FALSE;
				break;
			}
		}

		if( bCollide)
			return true;
	}
*/

	return false;
}

}