
//Vector3 inlineÇÔ¼öµé
inline XiahGameEngine::Vector3::Vector3()
{
	memset( this, 0, sizeof( Vector3));
}

inline XiahGameEngine::Vector3::Vector3(const XiahGameEngine::Vector3 &v)
{
	memcpy( this, &v, sizeof( Vector3));
}

inline XiahGameEngine::Vector3::Vector3(float fX,float fY,float fZ)
{
	x = fX;
	y = fY;
	z = fZ;
}

inline float &XiahGameEngine::Vector3::operator [] (int iAxis)
{
	return xyz[ iAxis];
}

inline bool XiahGameEngine::Vector3::operator < (const XiahGameEngine::Vector3 &v) const
{
	return x < v.x && y < v.y && z < v.z;
}

inline bool XiahGameEngine::Vector3::operator > (const XiahGameEngine::Vector3 &v) const
{
	return x > v.x && y > v.y && z > v.z;
}

inline bool XiahGameEngine::Vector3::operator == (const XiahGameEngine::Vector3 &v) const
{
	return x == v.x && y == v.y && z == v.z;
}

inline bool XiahGameEngine::Vector3::operator != (const XiahGameEngine::Vector3 &v) const
{
	return !(x == v.x && y == v.y && z == v.z);
}

inline XiahGameEngine::Vector3::operator float * ()
{
	return &x;
}

inline XiahGameEngine::Vector3::operator const float *() const
{
	return &x;
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator = (const XiahGameEngine::Vector3 &v)
{
	memcpy( this, &v, sizeof( Vector3));
	
	return *this;
}

inline void XiahGameEngine::Vector3::operator -= (const XiahGameEngine::Vector3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

inline void XiahGameEngine::Vector3::operator += (const XiahGameEngine::Vector3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

inline void XiahGameEngine::Vector3::operator /= (float fScalar)
{
	x /= fScalar;
	y /= fScalar;
	z /= fScalar;
}

inline void XiahGameEngine::Vector3::operator *= (float fScalar)
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator - (const XiahGameEngine::Vector3 &v) const
{
	return Vector3( x - v.x, y - v.y, z - v.z);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator + (const XiahGameEngine::Vector3 &v) const
{
	return Vector3( x + v.x, y + v.y, z + v.z);
}
inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator - (const XiahGameEngine::Vector3 &v)
{
	return Vector3( x - v.x, y - v.y, z - v.z);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator + (const XiahGameEngine::Vector3 &v)
{
	return Vector3( x + v.x, y + v.y, z + v.z);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator / (float fScalar) const
{
	return Vector3( x / fScalar, y / fScalar, z / fScalar);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator * (float fScalar) const
{
	return Vector3( x * fScalar, y * fScalar, z * fScalar);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator / (float fScalar)
{
	return Vector3( x / fScalar, y / fScalar, z / fScalar);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::operator * (float fScalar)
{
	return Vector3( x * fScalar, y * fScalar, z * fScalar);
}

inline void XiahGameEngine::Vector3::SetLength(float fLength)
{
	operator *= ( fLength / GetLength());
}

inline float XiahGameEngine::Vector3::GetLengthSqr() const
{
	return Dot( *this);
}

inline float XiahGameEngine::Vector3::GetLengthSqr()
{
	return Dot( *this);
}

inline float XiahGameEngine::Vector3::GetLength() const
{
	//return SQRT( x * x + y * y + z * z);

	// OPTIMIZE
	return nSQRT( x * x + y * y + z * z);
}

inline float XiahGameEngine::Vector3::GetLength()
{
	//return SQRT( x * x + y * y + z * z);
	// OPTIMIZE
	return nSQRT( x * x + y * y + z * z);
}

inline float XiahGameEngine::Vector3::GetAngle(const Vector3 &v)
{
	float fDot = Dot(v);
	float fMagnitude = (GetLength() * v.GetLength());
	return ACOS( fDot / fMagnitude);
}

inline float XiahGameEngine::Vector3::Dot(const Vector3 &v) const
{
	//return x * v.x + y * v.y + z * v.z;
	return nDot((float*)&xyz[0],(float*)&v[0]);
}

inline float XiahGameEngine::Vector3::Dot(const Vector3 &v)
{
//	return x * v.x + y * v.y + z * v.z;
	return nDot((float*)&xyz[0],(float*)&v[0]);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::Cross(const XiahGameEngine::Vector3 &v) const
{
	return Vector3( (y * v.z) - (z * v.y),
					(z * v.x) - (x * v.z),
					(x * v.y) - (y * v.x));
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::Cross(const XiahGameEngine::Vector3 &v)
{
	return Vector3( (y * v.z) - (z * v.y),
					(z * v.x) - (x * v.z),
					(x * v.y) - (y * v.x));
}

inline float XiahGameEngine::Vector3::Distance(const XiahGameEngine::Vector3 &v) const
{
	return (v - *this).GetLength();
}

inline float XiahGameEngine::Vector3::Distance(const XiahGameEngine::Vector3 &v)
{
	return (v - *this).GetLength();
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::Lerp(const XiahGameEngine::Vector3 &v, float fFactor) const
{
	return (*this + ((v - *this) * fFactor));
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::Lerp(const XiahGameEngine::Vector3 &v, float fFactor)
{
	return (*this + ((v - *this) * fFactor));
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::GetNormal() const
{
	return *this / GetLength();
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::GetNormal()
{
	return *this / GetLength();
}

inline void XiahGameEngine::Vector3::Normalize()
{
	/*float fLength = GetLength();
	x /= fLength;
	y /= fLength;
	z /= fLength;*/

	// OPTIMIZE
	nVectorNormalize(xyz);
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::Min(const XiahGameEngine::Vector3 &v)
{
	return Vector3( MIN( x, v.x),
					MIN( y, v.y),
					MIN( z, v.z));
}

inline XiahGameEngine::Vector3 XiahGameEngine::Vector3::Max(const XiahGameEngine::Vector3 &v)
{
	return Vector3( MAX( x, v.x),
					MAX( y, v.y),
					MAX( z, v.z));
}

inline XiahGameEngine::Vector3 XiahGameEngine::operator - (const XiahGameEngine::Vector3 &v)
{
	return Vector3( -v.x, -v.y, -v.z);
}

inline XiahGameEngine::Vector3 XiahGameEngine::operator - (XiahGameEngine::Vector3 &v)
{
	return Vector3( -v.x, -v.y, -v.z);
}

inline XiahGameEngine::Vector3 XiahGameEngine::operator * (float scalar,XiahGameEngine::Vector3 &v)
{
	return Vector3( v.x * scalar, v.y * scalar, v.z * scalar);
}

