
inline XiahGameEngine::Quaternion::Quaternion()
{
	x = y = z = 0;
	w = 1.0f;
}

inline XiahGameEngine::Quaternion::Quaternion(const XiahGameEngine::Quaternion &q)
{
	memcpy( this, &q, sizeof( Quaternion));
}

inline XiahGameEngine::Quaternion::Quaternion(const float fX,float fY,float fZ,float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

inline XiahGameEngine::Quaternion::Quaternion(const XiahGameEngine::Vector3 &vAxis,float Angle)
{
	SetAxisAngle( vAxis, Angle);
}

inline XiahGameEngine::Quaternion::Quaternion(const XiahGameEngine::Vector3 &vEuler)
{
	SetEuler( vEuler);
}

inline XiahGameEngine::Quaternion::operator float * (void)
{
	return &x;
}

inline void XiahGameEngine::Quaternion::operator *= (const XiahGameEngine::Quaternion &q)
{
	Quaternion tempQ;

	tempQ.x = ((w * q.x) + (x * q.w) + (y * q.z) - (z * q.y));
	tempQ.y = ((w * q.y) - (x * q.z) + (y * q.w) + (z * q.x));
	tempQ.z = ((w * q.z) + (x * q.y) - (y * q.x) + (z * q.w));
	tempQ.w = ((w * q.w) - (x * q.x) - (y * q.y) - (z * q.z));

	*this = tempQ;
}

inline XiahGameEngine::Quaternion &XiahGameEngine::Quaternion::operator = (const XiahGameEngine::Quaternion &q)
{
	memcpy( this, &q, sizeof( XiahGameEngine::Quaternion));

	return *this;
}

inline void XiahGameEngine::Quaternion::operator -= (const XiahGameEngine::Quaternion &q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
}

inline void XiahGameEngine::Quaternion::operator += (const XiahGameEngine::Quaternion &q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
}

inline void XiahGameEngine::Quaternion::operator /= (float fScalar)
{
	x /= fScalar;
	y /= fScalar;
	z /= fScalar;
	w /= fScalar;
}

inline void XiahGameEngine::Quaternion::operator *= (float fScalar)
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
	w *= fScalar;
}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::operator * (const XiahGameEngine::Quaternion &q)
{
	return Quaternion( ((w * q.x) + (x * q.w) + (y * q.z) - (z * q.y)),
					   ((w * q.y) - (x * q.z) + (y * q.w) + (z * q.x)),
					   ((w * q.z) + (x * q.y) - (y * q.x) + (z * q.w)),
					   ((w * q.w) - (x * q.x) - (y * q.y) - (z * q.z)));					   ;
}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::operator - (const XiahGameEngine::Quaternion &q)
{
	return Quaternion( x - q.x,
					   y - q.y,
					   z - q.z,
					   w - q.w);
}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::operator + (const XiahGameEngine::Quaternion &q)
{
	return Quaternion( x + q.x,
					   y + q.y,
					   z + q.z,
					   w + q.w);
}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::operator / (float fScalar)
{
	return Quaternion( x / fScalar,
					   y / fScalar,
					   z / fScalar,
					   w / fScalar);
}
inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::operator * (float fScalar)
{
	return Quaternion( x * fScalar,
					   y * fScalar,
					   z * fScalar,
					   w * fScalar);
}

inline void XiahGameEngine::Quaternion::Identity()
{
	x = y = z = 0;
	w = 1.0f;
}

inline void XiahGameEngine::Quaternion::SetEuler(const XiahGameEngine::Vector3 &v)
{
    float fSINx = SIN(v.x * 0.5f);
    float fSINy = SIN(v.y * 0.5f);
    float fSINz = SIN(v.z * 0.5f);
    float fCOSx = COS(v.x * 0.5f);
    float fCOSy = COS(v.y * 0.5f);
    float fCOSz = COS(v.z * 0.5f);
	float fSINyCOSz = (fSINy * fCOSz);
	float fSINySINz = (fSINy * fSINz);
	float fCOSyCOSz = (fCOSy * fCOSz);
	float fCOSySINz = (fCOSy * fSINz);

	x = ((fSINx * fCOSyCOSz) - (fCOSx * fSINySINz));
	y = ((fSINx * fCOSySINz) + (fCOSx * fSINyCOSz));
	z = ((fCOSx * fCOSySINz) - (fSINx * fSINyCOSz));
	w = ((fCOSx * fCOSyCOSz) + (fSINx * fSINySINz));
}

inline void XiahGameEngine::Quaternion::SetAxisAngle(const XiahGameEngine::Vector3 &vAxis,float fAngle)
{
	Vector3 normal;
	normal.Normalize();
	float fSinA = SIN( fAngle * 0.5f);

	x = (normal.x * fSinA);
	y = (normal.y * fSinA);
	z = (normal.z * fSinA);
	w = COS( fAngle * 0.5f);
}

inline float XiahGameEngine::Quaternion::Dot(const XiahGameEngine::Quaternion &q)
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

inline float XiahGameEngine::Quaternion::GetLengthSqr()
{
	return Dot( *this);
}

inline float XiahGameEngine::Quaternion::GetLength()
{
	//return SQRT( x * x + y * y + z * z + w * w);
	return nSQRT( x * x + y * y + z * z + w * w);
}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::GetNormal()
{
	float fLength = GetLength();

	return Quaternion( x / fLength,
					   y / fLength,
					   z / fLength,
					   w / fLength);
}

inline void XiahGameEngine::Quaternion::Normalize()
{
	float fLength = GetLength();

	x /= fLength;
	y /= fLength;
	z /= fLength;
	w /= fLength;
}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::Slerp(const XiahGameEngine::Quaternion &q,float fFactor)
{
/*
	float fDot = Dot( q);
	float fSqrt = SQRT( ABS( 1.0f - fDot * fDot));

	if( ABS( fSqrt) < ( _EPSILON * 100.0f)) return *this;
	if( fDot < 0.0f) fDot = -fDot;
	
	float fAngle = 0.0f;
	if( fDot != 0.0f) fAngle = atan( fSqrt / fDot);
	fSqrt = 1 / fSqrt;

	Quaternion qR;

	qR.x = ((x * fSqrt * sin(fAngle * (1.0f - fFactor))) + (q.x * fSqrt * sin(fAngle * fFactor)));
	qR.y = ((y * fSqrt * sin(fAngle * (1.0f - fFactor))) + (q.y * fSqrt * sin(fAngle * fFactor)));
	qR.z = ((z * fSqrt * sin(fAngle * (1.0f - fFactor))) + (q.z * fSqrt * sin(fAngle * fFactor)));
	qR.w = ((w * fSqrt * sin(fAngle * (1.0f - fFactor))) + (q.w * fSqrt * sin(fAngle * fFactor)));
	return qR;
*/

	float tol[ 4];

	double omega, cosom, sinom, scale0, scale1;

	cosom = x * q.x + y * q.y + z * q.z	+ w * q.w;

	if( cosom < 0.0f)
	{
		cosom = -cosom;
		tol[ 0] = - q.x;
		tol[ 1] = - q.y;
		tol[ 2] = - q.z;
		tol[ 3] = - q.w;
	}
	else
	{
		tol[ 0] = q.x;
		tol[ 1] = q.y;
		tol[ 2] = q.z;
		tol[ 3] = q.w;
	}

	if( ( 1.0 - cosom) > 0.0001 )
	{
		omega = acos( cosom);
		sinom = sin( omega);
		scale0 = sin( (1.0 - fFactor) * omega) / sinom;
		scale1 = sin( fFactor * omega) / sinom;

	}
	else
	{
		scale0 = 1.0f - fFactor;
		scale1 = fFactor;
	}

	Quaternion q3;

	q3.x = (float)(scale0 * x + scale1 * tol[ 0]);
	q3.y = (float)(scale0 * y + scale1 * tol[ 1]);
	q3.z = (float)(scale0 * z + scale1 * tol[ 2]);
	q3.w = (float)(scale0 * w + scale1 * tol[ 3]);

	return q3;

}

inline XiahGameEngine::Quaternion XiahGameEngine::Quaternion::Lerp(const XiahGameEngine::Quaternion &q,float fFactor)
{
	Quaternion qR;
	qR.x = (x + ((q.x - x) * fFactor));
	qR.y = (y + ((q.y - y) * fFactor));
	qR.z = (z + ((q.z - z) * fFactor));
	qR.w = (w + ((q.w - w) * fFactor));
	return qR;
}
