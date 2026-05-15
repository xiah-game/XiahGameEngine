
// Plane3 inline
inline XiahGameEngine::Plane3::Plane3()
{
	memset( this, 0, sizeof( Plane3));
}

inline XiahGameEngine::Plane3::Plane3(const XiahGameEngine::Plane3 &p)
{
	memcpy( this, &p, sizeof( Plane3));
}

inline XiahGameEngine::Plane3::Plane3(float fA,float fB,float fC,float fD)
{
	a = fA;
	b = fB;
	c = fC;
	d = fD;
}

inline XiahGameEngine::Plane3::Plane3(const XiahGameEngine::Vector3 &vNormal,float fDistance)
{
	n = vNormal;
	d = fDistance;
}

inline XiahGameEngine::Plane3::Plane3(const XiahGameEngine::Vector3 &a,const XiahGameEngine::Vector3 &b,const XiahGameEngine::Vector3 &c)
{
	n = Normalize( a, b, c);
	d = -n.Dot( a);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Plane3::Normalize(const XiahGameEngine::Vector3 &a,const XiahGameEngine::Vector3 &b,const XiahGameEngine::Vector3 &c)
{
	Vector3 vA = b - a;
	Vector3 vB = c - a;
	Vector3 vNormal( vA.Cross( vB));

	vNormal.Normalize();

	return vNormal;
}

inline XiahGameEngine::Plane3 &XiahGameEngine::Plane3::operator = (const XiahGameEngine::Plane3 &p)
{
	memcpy( this, &p, sizeof( Plane3));

	return *this;
}

inline float XiahGameEngine::Plane3::GetDistance(const XiahGameEngine::Vector3 &v) const
{
	return n.Dot( v) + d;
}

inline float XiahGameEngine::Plane3::GetDistance(const XiahGameEngine::Vector3 &v)
{
	return n.Dot( v) + d;
}

inline bool XiahGameEngine::Plane3::Intersect(const XiahGameEngine::Vector3 &vStart,const XiahGameEngine::Vector3 &vEnd)
{
	float fDistanceA = GetDistance( vStart);
	float fDistanceB = GetDistance( vEnd);

	if( fDistanceA * fDistanceB > 0) return false;

	return true;	// 값이 0인경우를 고려햇다.
}

inline bool XiahGameEngine::Plane3::Intersect(const XiahGameEngine::Vector3 &vStart,const XiahGameEngine::Vector3 &vEnd,XiahGameEngine::Vector3 &vCollide)
{
	float fDistanceA = GetDistance( vStart);
	float fDistanceB = GetDistance( vEnd);

	if( fDistanceA * fDistanceB > 0) return false;

	float alpha = ABS( fDistanceA) / (ABS( fDistanceA) + ABS( fDistanceB));

	vCollide = vStart + (vEnd - vStart) * alpha;

	return true;
}

inline bool XiahGameEngine::Plane3::IsFront(const XiahGameEngine::Vector3 &v) const
{
	return GetDistance( v) > 0.0f;
}

