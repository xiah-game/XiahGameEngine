// Frustum inline함수

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Frustum::Frustum()
{
	memset( this, 0, sizeof( Frustum));
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Frustum::Frustum(const Matrix4x4 &m)
{
	Set( m);
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Frustum::Set(const Matrix4x4 &m)
{
	m_Frustum[0].n.x = (m[ 3] + m[ 2]);
	m_Frustum[0].n.y = (m[ 7] + m[ 6]);
	m_Frustum[0].n.z = (m[11] + m[10]);
	m_Frustum[0].d =   (m[15] + m[14]);
	
// Extract the far plane
	m_Frustum[1].n.x = (m[ 3] - m[ 2]);
	m_Frustum[1].n.y = (m[ 7] - m[ 6]);
	m_Frustum[1].n.z = (m[11] - m[10]);
	m_Frustum[1].d =   (m[15] - m[14]);

// Extract the left plane
	m_Frustum[2].n.x = (m[ 3] + m[ 0]);
	m_Frustum[2].n.y = (m[ 7] + m[ 4]);
	m_Frustum[2].n.z = (m[11] + m[ 8]);
	m_Frustum[2].d =   (m[15] + m[12]);

// Extract the right plane
	m_Frustum[3].n.x = (m[ 3] - m[ 0]);
	m_Frustum[3].n.y = (m[ 7] - m[ 4]);
	m_Frustum[3].n.z = (m[11] - m[ 8]);
	m_Frustum[3].d =   (m[15] - m[12]);

// Extract the bottom plane
	m_Frustum[4].n.x = (m[ 3] + m[ 1]);
	m_Frustum[4].n.y = (m[ 7] + m[ 5]);
	m_Frustum[4].n.z = (m[11] + m[ 9]);
	m_Frustum[4].d =   (m[15] + m[13]);

// Extract the top plane
	m_Frustum[5].n.x = (m[ 3] - m[ 1]);
	m_Frustum[5].n.y = (m[ 7] - m[ 5]);
	m_Frustum[5].n.z = (m[11] - m[ 9]);
	m_Frustum[5].d =   (m[15] - m[13]);
}

//---------------------------------------------------------------------------------------
inline bool XiahGameEngine::Frustum::Visible(const XiahGameEngine::Vector3 &pos) const
{
	if( m_Frustum[ 0].GetDistance( pos) <= 0.0f) return false;
	if( m_Frustum[ 1].GetDistance( pos) <= 0.0f) return false;
	if( m_Frustum[ 2].GetDistance( pos) <= 0.0f) return false;
	if( m_Frustum[ 3].GetDistance( pos) <= 0.0f) return false;
	if( m_Frustum[ 4].GetDistance( pos) <= 0.0f) return false;
	if( m_Frustum[ 5].GetDistance( pos) <= 0.0f) return false;

	return true;
}

//---------------------------------------------------------------------------------------
inline bool XiahGameEngine::Frustum::Visible(const XiahGameEngine::Sphere3 &sphere) const
{
	for(int iIndex = 0; iIndex < 6; ++iIndex)
	{
		float fDistance = m_Frustum[ iIndex].GetDistance( sphere.m_vOrigin);
		if( fDistance <= - sphere.m_fRadius)
			return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------
/*
	BBox박스가 카메라의 Destination Plane보다 작은 경우에만 해당되는 알고리즘이다.

	그래서 몇가지 추가적인 부분을 첨가 햇다
*/
inline bool XiahGameEngine::Frustum::Visible(const XiahGameEngine::BBoxAABB3 &bbox) const
{
/*	
	Vector3 vPoint;

	BOOL bInside = TRUE;

	// 완전히 포함되었는가?
	for(int iIndex = 0; iIndex < 6; iIndex ++)
	{
		vPoint.x = ((m_Frustum[iIndex].n.x < 0.0f) ? bbox.m_vMin.x : bbox.m_vMax.x);
		vPoint.y = ((m_Frustum[iIndex].n.y < 0.0f) ? bbox.m_vMin.y : bbox.m_vMax.y);
		vPoint.z = ((m_Frustum[iIndex].n.z < 0.0f) ? bbox.m_vMin.z : bbox.m_vMax.z);
		if( m_Frustum[ iIndex].GetDistance( vPoint) <= 0.0f) 
		{
			bInside = FALSE;
			break;
		}
	}
	if( bInside)
		return true;

	// 각각의 Edge가 Plane Face를 통과하고 있는지
	return false;
*/
	Vector3 vCenter = (bbox.m_vMin + bbox.m_vMax) / 2;
	
	if( Visible( vCenter))
		return true;

	Vector3 vPoint;

	//BOOL bInside = TRUE;

	// 완전히 포함되었는가?
	for(int iIndex = 0; iIndex < 6; ++iIndex)
	{
		vPoint.x = ((m_Frustum[iIndex].n.x < 0.0f) ? bbox.m_vMin.x : bbox.m_vMax.x);
		vPoint.y = ((m_Frustum[iIndex].n.y < 0.0f) ? bbox.m_vMin.y : bbox.m_vMax.y);
		vPoint.z = ((m_Frustum[iIndex].n.z < 0.0f) ? bbox.m_vMin.z : bbox.m_vMax.z);

		if( m_Frustum[ iIndex].GetDistance( vPoint) <= 0.0f) 
		{
			return false;

			//bInside = FALSE;
			break;
		}
	}

	//if( bInside)
		return true;

	// 각각의 Edge가 Plane Face를 통과하고 있는지
	//return false;
}

//---------------------------------------------------------------------------------------
// 더 정확한건 쓸모가 없을것 같다
inline bool XiahGameEngine::Frustum::Visible(const XiahGameEngine::BBoxOBB3& box) const
{
	return Visible( box.m_BBoxAABB);
}