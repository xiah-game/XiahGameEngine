// Vector4 Inline ÇÔ¼ö

inline XiahGameEngine::Vector4::Vector4()
{
	memset( this, 0, sizeof( Vector4));
}
inline XiahGameEngine::Vector4::Vector4(const XiahGameEngine::Vector4 &v)
{
	memcpy( this, &v, sizeof( Vector4));
}

inline XiahGameEngine::Vector4::Vector4(float fX,float fY,float fZ,float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

inline float &XiahGameEngine::Vector4::operator [] (int iAxis)
{
	return xyzw[ iAxis];
}

inline bool XiahGameEngine::Vector4::operator < (const XiahGameEngine::Vector4 &v) const
{
	return x < v.x && y < v.y && z < v.z && w < v.w;
}

inline bool XiahGameEngine::Vector4::operator > (const XiahGameEngine::Vector4 &v) const
{
	return x > v.x && y > v.y && z > v.z && w > v.w;
}

inline bool XiahGameEngine::Vector4::operator == (const XiahGameEngine::Vector4 &v) const
{
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

inline bool XiahGameEngine::Vector4::operator != (const XiahGameEngine::Vector4 &v) const
{
	return !(x == v.x && y == v.y && z == v.z && w == v.w);
}

inline XiahGameEngine::Vector4::operator float *()
{
	return &x;
}

inline XiahGameEngine::Vector4::operator const float *() const
{
	return &x;
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator = (const XiahGameEngine::Vector4 &v)
{
	memcpy( this, &v, sizeof( Vector4));
	
	return *this;
}

inline void XiahGameEngine::Vector4::operator -= (const XiahGameEngine::Vector4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

inline void XiahGameEngine::Vector4::operator += (const XiahGameEngine::Vector4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

inline void XiahGameEngine::Vector4::operator /= (float fScalar)
{
	x /= fScalar;
	y /= fScalar;
	z /= fScalar;
	w /= fScalar;
}

inline void XiahGameEngine::Vector4::operator *= (float fScalar)
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
	w *= fScalar;
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator - (const XiahGameEngine::Vector4 &v) const
{
	return Vector4( x - v.x, y - v.y, z - v.z, w - v.w);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator + (const XiahGameEngine::Vector4 &v) const
{
	return Vector4( x + v.x, y + v.y, z + v.z, w + v.w);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator - (const XiahGameEngine::Vector4 &v)
{
	return Vector4( x - v.x, y - v.y, z - v.z, w - v.w);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator + (const XiahGameEngine::Vector4 &v)
{
	return Vector4( x + v.x, y + v.y, z + v.z, w + v.w);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator / (float fScalar) const
{
	return Vector4( x / fScalar, y / fScalar, z / fScalar, w / fScalar);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator * (float fScalar) const
{
	return Vector4( x * fScalar, y * fScalar, z * fScalar, w * fScalar);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator / (float fScalar)
{
	return Vector4( x / fScalar, y / fScalar, z / fScalar, w / fScalar);
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::operator * (float fScalar)
{
	return Vector4( x * fScalar, y * fScalar, z * fScalar, w * fScalar);
}

inline void XiahGameEngine::Vector4::SetLength(float fLength)
{
	operator *= ( fLength / GetLength());
}

inline float XiahGameEngine::Vector4::GetLengthSqr() const
{
	return Dot( *this);
}

inline float XiahGameEngine::Vector4::GetLengthSqr()
{
	return Dot( *this);
}

inline float XiahGameEngine::Vector4::GetLength() const
{
	//return SQRT( x * x + y * y + z * z + w * w);

	// OPTIMIZE
	return nSQRT( x * x + y * y + z * z + w * w);
}

inline float XiahGameEngine::Vector4::GetLength()
{
	//return SQRT( x * x + y * y + z * z + w * w);

	// OPTIMIZE
	return nSQRT( x * x + y * y + z * z + w * w);
}

inline float XiahGameEngine::Vector4::GetAngle(const XiahGameEngine::Vector4 &v)
{
	float fDot = Dot(v);
	float fMagnitude = (GetLength() * v.GetLength());
	return ACOS( fDot / fMagnitude);
}

inline float XiahGameEngine::Vector4::Dot(const XiahGameEngine::Vector4 &v) const
{
	//return x * v.x + y * v.y + z * v.z + w * v.w;
	return nDot4((float*)&xyzw[0],(float*)&v[0]);
}

inline float XiahGameEngine::Vector4::Dot(const XiahGameEngine::Vector4 &v)
{
	//return x * v.x + y * v.y + z * v.z + w * v.w;
	return nDot4((float*)&xyzw[0],(float*)&v[0]);
}

inline float XiahGameEngine::Vector4::Distance(const XiahGameEngine::Vector4 &v) const
{
	return (v - *this).GetLength();
}

inline float XiahGameEngine::Vector4::Distance(const XiahGameEngine::Vector4 &v)
{
	return (v - *this).GetLength();
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::Lerp(const XiahGameEngine::Vector4 &v, float fFactor) const
{
	return (*this + ((v - *this) * fFactor));
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::Lerp(const XiahGameEngine::Vector4 &v, float fFactor)
{
	return (*this + ((v - *this) * fFactor));
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::GetNormal() const
{
	return *this / GetLength();
}

inline XiahGameEngine::Vector4 XiahGameEngine::Vector4::GetNormal()
{
	return *this / GetLength();
}

inline void XiahGameEngine::Vector4::Normalize()
{
	float fLength = GetLength();
	x /= fLength;
	y /= fLength;
	z /= fLength;
	w /= fLength;
}

inline XiahGameEngine::Vector4 XiahGameEngine::operator - (const XiahGameEngine::Vector4 &v)
{
	return Vector4( -v.x, -v.y, -v.z, -v.w);
}

inline XiahGameEngine::Vector4 XiahGameEngine::operator - (XiahGameEngine::Vector4 &v)
{
	return Vector4( -v.x, -v.y, -v.z, -v.w);
}

inline XiahGameEngine::Vector4 XiahGameEngine::operator * (float scalar,XiahGameEngine::Vector4 &v)
{
	return Vector4( v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}
