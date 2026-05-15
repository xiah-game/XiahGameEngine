namespace XiahGameEngine
{
	//---------------------------------------------------------------------------------------
	inline Face3::Face3()
	{
		ZeroMemory( this, sizeof( Face3));
	}

	//---------------------------------------------------------------------------------------
	inline Face3::Face3(const Face3& face)
	{
		CopyMemory( this, &face, sizeof( Face3));
	}

	//---------------------------------------------------------------------------------------
	inline Face3::Face3(const Vector3& a,const Vector3& b,const Vector3& c)
	{
		v[ 0] = a;
		v[ 1] = b;
		v[ 2] = c;

		plane = Plane3( a, b, c);
	}

	//---------------------------------------------------------------------------------------
	inline bool Face3::IsIntersect(const Vector3& p1,const Vector3& p2)
	{
		Vector3 vCollide;

		if( !plane.Intersect( p1, p2, vCollide))
			return false;

		Vector3 vLine,vLineNormal, vPToV;
		float dp;
		
		vLine = v[ 1] - v[ 0];
		vLineNormal = plane.n.Cross( vLine);
		vPToV = vCollide - v[ 0];

		dp = vLineNormal.Dot( vPToV);
		if( dp < 0)
			return false;
		
		
		vLine = v[ 2] - v[ 1];
		vLineNormal = plane.n.Cross( vLine);
		vPToV = vCollide - v[ 1];
		
		dp = vLineNormal.Dot( vPToV);
		if( dp < 0)
			return false;

		vLine = v[ 0] - v[ 2];
		vLineNormal = plane.n.Cross( vLine);
		vPToV = vCollide - v[ 2];
		dp = vLineNormal.Dot( vPToV);
		if(dp < 0)
			return false;

		return true;
	}

	//---------------------------------------------------------------------------------------
	inline Vector3 Face3::GetIntersectPoint(const Vector3& p1,const Vector3& p2)
	{
		Vector3 vCollide;

		plane.Intersect( p1, p2, vCollide);

		return vCollide;
	}

};
