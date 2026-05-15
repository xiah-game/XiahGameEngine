
// Vector2 InlineÇÔ¼öµé
inline XiahGameEngine::Vector2::Vector2()
{
	memset( this, 0, sizeof( Vector2));
}

inline XiahGameEngine::Vector2::Vector2(const XiahGameEngine::Vector2 &v)
{
	memcpy( this, &v, sizeof( Vector2));
}

inline XiahGameEngine::Vector2::Vector2(float x,float y)
{
	xy[ 0] = x;
	xy[ 1] = y;
}

inline float &XiahGameEngine::Vector2::operator [] (int iAxis)
{
	return xy[ iAxis];
}

inline bool XiahGameEngine::Vector2::operator < (const XiahGameEngine::Vector2 &v) const
{
	return x < v.x && y < v.y;
}

inline bool XiahGameEngine::Vector2::operator > (const XiahGameEngine::Vector2 &v) const
{
	return x > v.x && y > v.y;
}

inline bool XiahGameEngine::Vector2::operator == (const XiahGameEngine::Vector2 &v) const
{
	return x == v.x && y == v.y;
}

inline bool XiahGameEngine::Vector2::operator != (const XiahGameEngine::Vector2 &v) const
{
	return x == v.x && y == v.y;
}

inline XiahGameEngine::Vector2::operator float *()
{
	return &x;
}

inline XiahGameEngine::Vector2::operator const float *() const
{
	return &x;
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator = (const XiahGameEngine::Vector2 &v)
{
	memcpy( this, &v, sizeof( Vector2));
	return *this;
}

inline void XiahGameEngine::Vector2::operator -= (const XiahGameEngine::Vector2 &v)
{
	x -= v.x;
	y -= v.y;
}

inline void XiahGameEngine::Vector2::operator += (const XiahGameEngine::Vector2 &v)
{
	x += v.x;
	y += v.y;
}

inline void XiahGameEngine::Vector2::operator /= (float fScalar)
{
	x /= fScalar;
	y /= fScalar;
}

inline void XiahGameEngine::Vector2::operator *= (float fScalar)
{
	x /= fScalar;
	y /= fScalar;
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator - (const XiahGameEngine::Vector2 &v) const
{
	return Vector2( x - v.x, y - v.y);	
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator + (const XiahGameEngine::Vector2 &v) const
{
	return Vector2( x + v.x, y + v.y);
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator - (const XiahGameEngine::Vector2 &v)
{
	return Vector2( x - v.x, y - v.y);
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator + (const XiahGameEngine::Vector2 &v)
{
	return Vector2( x + v.x, y + v.y);
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator / (float fScalar) const
{
	return Vector2( x / fScalar, y / fScalar);
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator * (float fScalar) const
{
	return Vector2( x * fScalar, y / fScalar);
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator / (float fScalar)
{
	return Vector2( x / fScalar, y / fScalar);
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::operator * (float fScalar)
{
	return Vector2( x * fScalar, y / fScalar);
}

inline void XiahGameEngine::Vector2::SetLength(float fLength)
{
	operator *= ( fLength / GetLength());
}

inline float XiahGameEngine::Vector2::GetLengthSqr() const
{
	return Dot(*this);
}

inline float XiahGameEngine::Vector2::GetLengthSqr()
{
	return Dot(*this);
}

inline float XiahGameEngine::Vector2::GetLength() const
{
	// return SQRT( x * x + y * y);

	// OPTIMIZE
	return nSQRT( x * x + y * y);
}

inline float XiahGameEngine::Vector2::GetLength()
{
	//return SQRT( x * x + y * y);

	// OPTIMIZE
	return nSQRT( x * x + y * y);
}

inline float XiahGameEngine::Vector2::GetAngle(const XiahGameEngine::Vector2 &v)
{
	float fDot = Dot(v);
	float fMagnitude = (GetLength() * v.GetLength());
	return ACOS( fDot / fMagnitude);
}

inline float XiahGameEngine::Vector2::Distance(const XiahGameEngine::Vector2 &v) const
{
	return (v - *this).GetLength();
}

inline float XiahGameEngine::Vector2::Distance(const XiahGameEngine::Vector2 &v)
{
	return (v - *this).GetLength();
}

inline float XiahGameEngine::Vector2::Dot(const XiahGameEngine::Vector2 &v) const
{
	return x * v.x + y * v.y;
}

inline float XiahGameEngine::Vector2::Dot(const XiahGameEngine::Vector2 &v)
{
	return x * v.x + y * v.y;
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::Lerp(const XiahGameEngine::Vector2 &v,float fFactor) const
{
	return (*this + ((v - *this) * fFactor));
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::Lerp(const XiahGameEngine::Vector2 &v,float fFactor)
{
	return (*this + ((v - *this) * fFactor));
}

inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::GetNormal() const
{
	return *this / GetLength();
}
inline XiahGameEngine::Vector2 XiahGameEngine::Vector2::GetNormal()
{
	return *this / GetLength();
}

inline void XiahGameEngine::Vector2::Normalize()
{
	float fLength = GetLength();
	x /= fLength;
	y /= fLength;
}

inline XiahGameEngine::Vector2 XiahGameEngine::operator - (const XiahGameEngine::Vector2 &v)
{
	return Vector2( -v.x, -v.y);
}

inline XiahGameEngine::Vector2 XiahGameEngine::operator - (XiahGameEngine::Vector2 &v)
{
	return Vector2( -v.x, -v.y);
}

inline XiahGameEngine::Vector2 XiahGameEngine::operator * (float scalar,XiahGameEngine::Vector2 &v)
{
	return Vector2( v.x * scalar, v.y * scalar);
}

