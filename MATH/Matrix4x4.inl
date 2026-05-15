// Matrix4x4 inline함수

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4::Matrix4x4()
{
	Identity();
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4::Matrix4x4(float *pfData)
{
	memcpy( m, pfData, sizeof(float) * 16);
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4::Matrix4x4(float f11,float f12,float f13,float f14,
					float f21,float f22,float f23,float f24,
					float f31,float f32,float f33,float f34,
					float f41,float f42,float f43,float f44)
{
	_11 = f11;_12 = f12;_13 = f13;_14 = f14;
	_21 = f21;_22 = f22;_23 = f23;_24 = f24;
	_31 = f31;_32 = f32;_33 = f33;_34 = f34;
	_41 = f41;_42 = f42;_43 = f43;_44 = f44;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4::Matrix4x4(const Matrix4x4 &m)
{
	memcpy( this, &m, sizeof( Matrix4x4));
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4 &XiahGameEngine::Matrix4x4::operator = (const Matrix4x4 &m)
{
	memcpy( this, &m, sizeof( Matrix4x4));
	return *this;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::operator *= (const XiahGameEngine::Matrix4x4 &m)
{
/*
	*this = Matrix4x4(  ((_11 * m._11) + (_12 * m._21) + (_13 * m._31) + (_14 * m._41)),
						((_11 * m._12) + (_12 * m._22) + (_13 * m._32) + (_14 * m._42)),
						((_11 * m._13) + (_12 * m._23) + (_13 * m._33) + (_14 * m._43)),
						((_11 * m._14) + (_12 * m._24) + (_13 * m._34) + (_14 * m._44)),

						((_21 * m._11) + (_22 * m._21) + (_23 * m._31) + (_24 * m._41)),
						((_21 * m._12) + (_22 * m._22) + (_23 * m._32) + (_24 * m._42)),
						((_21 * m._13) + (_22 * m._23) + (_23 * m._33) + (_24 * m._43)),
						((_21 * m._14) + (_22 * m._24) + (_23 * m._34) + (_24 * m._44)),

						((_31 * m._11) + (_32 * m._21) + (_33 * m._31) + (_34 * m._41)),
						((_31 * m._12) + (_32 * m._22) + (_33 * m._32) + (_34 * m._42)),
						((_31 * m._13) + (_32 * m._23) + (_33 * m._33) + (_34 * m._43)),
						((_31 * m._14) + (_32 * m._24) + (_33 * m._34) + (_34 * m._44)),

						((_41 * m._11) + (_42 * m._21) + (_43 * m._31) + (_44 * m._41)),
						((_41 * m._12) + (_42 * m._22) + (_43 * m._32) + (_44 * m._42)),
						((_41 * m._13) + (_42 * m._23) + (_43 * m._33) + (_44 * m._43)),
						((_41 * m._14) + (_42 * m._24) + (_43 * m._34) + (_44 * m._44)));
*/
	Matrix4x4 temp( *this);

	_11 = ((temp._11 * m._11) + (temp._12 * m._21) + (temp._13 * m._31) + (temp._14 * m._41));
	_12 = ((temp._11 * m._12) + (temp._12 * m._22) + (temp._13 * m._32) + (temp._14 * m._42));
	_13 = ((temp._11 * m._13) + (temp._12 * m._23) + (temp._13 * m._33) + (temp._14 * m._43));
	_14 = ((temp._11 * m._14) + (temp._12 * m._24) + (temp._13 * m._34) + (temp._14 * m._44));

	_21 = ((temp._21 * m._11) + (temp._22 * m._21) + (temp._23 * m._31) + (temp._24 * m._41));
	_22 = ((temp._21 * m._12) + (temp._22 * m._22) + (temp._23 * m._32) + (temp._24 * m._42));
	_23 = ((temp._21 * m._13) + (temp._22 * m._23) + (temp._23 * m._33) + (temp._24 * m._43));
	_24 = ((temp._21 * m._14) + (temp._22 * m._24) + (temp._23 * m._34) + (temp._24 * m._44));

	_31 = ((temp._31 * m._11) + (temp._32 * m._21) + (temp._33 * m._31) + (temp._34 * m._41));
	_32 = ((temp._31 * m._12) + (temp._32 * m._22) + (temp._33 * m._32) + (temp._34 * m._42));
	_33 = ((temp._31 * m._13) + (temp._32 * m._23) + (temp._33 * m._33) + (temp._34 * m._43));
	_34 = ((temp._31 * m._14) + (temp._32 * m._24) + (temp._33 * m._34) + (temp._34 * m._44));

	_41 = ((temp._41 * m._11) + (temp._42 * m._21) + (temp._43 * m._31) + (temp._44 * m._41));
	_42 = ((temp._41 * m._12) + (temp._42 * m._22) + (temp._43 * m._32) + (temp._44 * m._42));
	_43 = ((temp._41 * m._13) + (temp._42 * m._23) + (temp._43 * m._33) + (temp._44 * m._43));
	_44 = ((temp._41 * m._14) + (temp._42 * m._24) + (temp._43 * m._34) + (temp._44 * m._44));
}

//---------------------------------------------------------------------------------------
inline float XiahGameEngine::Matrix4x4::operator () (int iRow,int iCol) const
{
	return m[ iRow][ iCol];
}

//---------------------------------------------------------------------------------------
inline float XiahGameEngine::Matrix4x4::operator () (int iRow,int iCol)
{
	return m[ iRow][ iCol];
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4::operator const float * () const
{
	return &_11;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4::operator float * ()
{
	return &_11;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4 XiahGameEngine::Matrix4x4::operator * (const XiahGameEngine::Matrix4x4 &m)
{
	Matrix4x4 r;
	r._11 = ((_11 * m._11) + (_12 * m._21) + (_13 * m._31) + (_14 * m._41));
	r._12 = ((_11 * m._12) + (_12 * m._22) + (_13 * m._32) + (_14 * m._42));
	r._13 = ((_11 * m._13) + (_12 * m._23) + (_13 * m._33) + (_14 * m._43));
	r._14 = ((_11 * m._14) + (_12 * m._24) + (_13 * m._34) + (_14 * m._44));

	r._21 = ((_21 * m._11) + (_22 * m._21) + (_23 * m._31) + (_24 * m._41));
	r._22 = ((_21 * m._12) + (_22 * m._22) + (_23 * m._32) + (_24 * m._42));
	r._23 = ((_21 * m._13) + (_22 * m._23) + (_23 * m._33) + (_24 * m._43));
	r._24 = ((_21 * m._14) + (_22 * m._24) + (_23 * m._34) + (_24 * m._44));

	r._31 = ((_31 * m._11) + (_32 * m._21) + (_33 * m._31) + (_34 * m._41));
	r._32 = ((_31 * m._12) + (_32 * m._22) + (_33 * m._32) + (_34 * m._42));
	r._33 = ((_31 * m._13) + (_32 * m._23) + (_33 * m._33) + (_34 * m._43));
	r._34 = ((_31 * m._14) + (_32 * m._24) + (_33 * m._34) + (_34 * m._44));

	r._41 = ((_41 * m._11) + (_42 * m._21) + (_43 * m._31) + (_44 * m._41));
	r._42 = ((_41 * m._12) + (_42 * m._22) + (_43 * m._32) + (_44 * m._42));
	r._43 = ((_41 * m._13) + (_42 * m._23) + (_43 * m._33) + (_44 * m._43));
	r._44 = ((_41 * m._14) + (_42 * m._24) + (_43 * m._34) + (_44 * m._44));

	return r;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::Identity()
{
	memset( m, 0, sizeof( Matrix4x4));
	_11 = _22 = _33 = _44 = 1.0f;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::Zero()
{
	memset( m, 0, sizeof( Matrix4x4));
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationEuler(const XiahGameEngine::Vector3 &v)
{
	float a,b,c,d,e,f;

	a = COS( v.x);
	b = SIN( v.x);
	c = COS( v.y);
	d = SIN( v.y);
	e = COS( v.z);
	f = SIN( v.z);

	float ad = a * d;
	float bd = b * d;

	m[0][0] =   c * e;
	m[0][1] =  -c * f;
	m[0][2] =  -d;
	m[1][0] = -bd * e + a * f;
	m[1][1] =  bd * f + a * e;
	m[1][2] =  -b * c;
	m[2][0] =  ad * e + b * f;
	m[2][1] = -ad * f + b * e;
	m[2][2] =   a * c;

	m[0][3] =  m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] =  1;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationAxisAngle(const XiahGameEngine::Vector3 &v,float fAngle)
{
	XiahGameEngine::Vector3 vNormal( v.GetNormal());

	float c = COS( -fAngle);
	float s = SIN( -fAngle);
	float t = 1 - c;

    m[0][0] = (t * vNormal.x * vNormal.x) + c;
    m[1][1] = (t * vNormal.y * vNormal.y) + c;
    m[2][2] = (t * vNormal.z * vNormal.z) + c;

    m[1][0] = (t * vNormal.x * vNormal.y) + s * vNormal.z;
    m[2][0] = (t * vNormal.x * vNormal.z) - s * vNormal.y;
    m[0][1] = (t * vNormal.x * vNormal.y) - s * vNormal.z;
    
    m[2][1] = (t * vNormal.y * vNormal.z) + s * vNormal.x;
    m[0][2] = (t * vNormal.x * vNormal.z) + s * vNormal.y;
    m[1][2] = (t * vNormal.y * vNormal.z) - s * vNormal.x;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationQuaternion(const XiahGameEngine::Quaternion &q)
{
	float f2X = (q.x * 2.0f);
	float f2Y = (q.y * 2.0f);
	float f2Z = (q.z * 2.0f);
	float fWX = (q.w * f2X);
	float fWY = (q.w * f2Y);
	float fWZ = (q.w * f2Z);
	float fXX = (q.x * f2X);
	float fXY = (q.x * f2Y);
	float fXZ = (q.x * f2Z);
	float fYY = (q.y * f2Y);
	float fYZ = (q.y * f2Z);
	float fZZ = (q.z * f2Z);
	
	_11 = (1.0f - fYY - fZZ);
	_12 = (fXY + fWZ);
	_13 = (fXZ - fWY);

	_21 = (fXY - fWZ);
	_22 = (1.0f - fXX - fZZ);
	_23 = (fYZ + fWX);

	_31 = (fXZ + fWY);
	_32 = (fYZ - fWX);
	_33 = (1.0f - fXX - fYY);

	_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
	_44 = 1.0f;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationQuaternion_Flip(const XiahGameEngine::Quaternion &q,bool bFlip)
{
	// quaternion값이 뒤집어진형태로 들어 온다.
	// flip이 -1인 형태가 정상인 형태다
	float xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;
    
    xs = x * 2.0f;	ys = y * 2.0f;	zs = z * 2.0f;
    wx = w * xs;	wy = w * ys;	wz = w * zs;
    xx = x * xs;	xy = x * ys;	xz = x * zs;
    yy = y * ys;	yz = y * zs;	zz = z * zs;

	if( !bFlip)
	{
		m[0][0] = (1.0f - yy - zz);
		m[0][1] = (xy - wz);
		m[0][2] = (xz + wy);

		m[1][0] = (xy + wz);
		m[1][1] = (1.0f - xx - zz);
		m[1][2] = (yz - wx);

		m[2][0] = (xz - wy);
		m[2][1] = (yz + wx);
		m[2][2] = (1.0f - xx - yy);
	}
	else
	{
		m[0][0] = -(1.0f - yy - zz);
		m[0][1] = -(xy - wz);
		m[0][2] = -(xz + wy);

		m[1][0] = -(xy + wz);
		m[1][1] = -(1.0f - xx - zz);
		m[1][2] = -(yz - wx);

		m[2][0] = -(xz - wy);
		m[2][1] = -(yz + wx);
		m[2][2] = -(1.0f - xx - yy);
	}
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationX(float fAngle)
{
	float s = SIN( fAngle);
	float c = COS( fAngle);

	Identity();

	m[ 1][ 1] = c;
	m[ 1][ 2] = s;
	m[ 2][ 1] = -s;
	m[ 2][ 2] = c;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationY(float fAngle)
{
	float s = SIN( fAngle);
	float c = COS( fAngle);

	Identity();

	m[ 0][ 0] = c;
	m[ 0][ 2] = s;
	m[ 2][ 0] = -s;
	m[ 2][ 2] = c;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationZ(float fAngle)
{
	float s = SIN( fAngle);
	float c = COS( fAngle);

	Identity();

	m[ 0][ 0] = c;
	m[ 0][ 1] = s;
	m[ 1][ 0] = -s;
	m[ 1][ 1] = c;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetRotationTarget(const XiahGameEngine::Vector3 &vOrigin,const XiahGameEngine::Vector3 &vTarget)
{
	XiahGameEngine::Vector3 vZAxis( vTarget - vOrigin);
	vZAxis.Normalize();

	XiahGameEngine::Vector3 vTempYAxis( 0.0f, 1.0f, 0.0f);
	XiahGameEngine::Vector3 vXAxis( vTempYAxis.Cross( vZAxis));
	vXAxis.Normalize();

	XiahGameEngine::Vector3 vYAxis( vZAxis.Cross( vXAxis));

	*this = Matrix4x4(vXAxis.x, vXAxis.y, vXAxis.z, -vXAxis.Dot(vOrigin),
					 vYAxis.x, vYAxis.y, vYAxis.z, -vYAxis.Dot(vOrigin),
					 vZAxis.x, vZAxis.y, vZAxis.z, -vZAxis.Dot(vOrigin),
					 0.0f, 0.0f, 0.0f, 1.0f);
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetScale(const XiahGameEngine::Vector3 &v)
{
	Identity();

	_11 = v.x;
	_22 = v.y;
	_33 = v.z;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetScale(float fScale)
{
	Identity();

	_11 = _22 = _33 = fScale;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::Matrix4x4::GetScale()
{
	return Vector3( _11, _22, _33);
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::Transpose()
{
	Matrix4x4 temp( *this);

    _11 = temp._11;    _12 = temp._21;    _13 = temp._31;    _14 = temp._41;
    _21 = temp._12;    _22 = temp._22;    _23 = temp._32;    _24 = temp._42;
    _31 = temp._13;    _32 = temp._23;    _33 = temp._33;    _34 = temp._43;
    _41 = temp._14;    _42 = temp._24;    _43 = temp._34;    _44 = temp._44;
}

//---------------------------------------------------------------------------------------
inline float XiahGameEngine::Matrix4x4::Determinant()
{
    return ((_11 * ((_22 * _33) - (_23 * _32))) -
            (_12 * ((_21 * _33) - (_23 * _31))) +
            (_13 * ((_21 * _32) - (_22 * _31))));
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Matrix4x4 XiahGameEngine::Matrix4x4::GetInverse()
{
	Matrix4x4 mR;
	float fDetInverse = (1.0f / Determinant());

    mR._11 = ( fDetInverse * ((_22 * _33) - (_23 * _32)));
    mR._12 = (-fDetInverse * ((_12 * _33) - (_13 * _32)));
    mR._13 = ( fDetInverse * ((_12 * _23) - (_13 * _22)));
    mR._14 = 0.0f;

    mR._21 = (-fDetInverse * ((_21 * _33) - (_23 * _31)));
    mR._22 = ( fDetInverse * ((_11 * _33) - (_13 * _31)));
    mR._23 = (-fDetInverse * ((_11 * _23) - (_13 * _21)));
    mR._24 = 0.0f;

    mR._31 = ( fDetInverse * ((_21 * _32) - (_22 * _31)));
    mR._32 = (-fDetInverse * ((_11 * _32) - (_12 * _31)));
    mR._33 = ( fDetInverse * ((_11 * _22) - (_12 * _21)));
    mR._34 = 0.0f;

    mR._41 = -((_41 * mR._11) + (_42 * mR._21) + (_43 * mR._31));
    mR._42 = -((_41 * mR._12) + (_42 * mR._22) + (_43 * mR._32));
    mR._43 = -((_41 * mR._13) + (_42 * mR._23) + (_43 * mR._33));
    mR._44 = 1.0f;

	return mR;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::Inverse()
{
	*this = GetInverse();
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetViewMatrix(XiahGameEngine::Vector3 &vFrom,XiahGameEngine::Vector3 &vAt,XiahGameEngine::Vector3 &vUp)
{
	XiahGameEngine::Vector3 vView = vAt - vFrom;
	vView.Normalize();

	float fDotProduct = vUp.Dot( vView);

	XiahGameEngine::Vector3 vViewUp = vUp - fDotProduct * vView;

	vUp.Normalize();

	XiahGameEngine::Vector3 vRight;

	vRight = vUp.Cross( vView);

	m[ 0][ 0] = vRight.x;
	m[ 1][ 0] = vRight.y;
	m[ 2][ 0] = vRight.z;

	m[ 0][ 1] = vViewUp.x;
	m[ 1][ 1] = vViewUp.y;
	m[ 2][ 1] = vViewUp.z;

	m[ 0][ 2] = vView.x;
	m[ 1][ 2] = vView.y;
	m[ 2][ 2] = vView.z;

	m[ 3][ 0] = - vRight.Dot( vFrom);
	m[ 3][ 1] = - vViewUp.Dot( vFrom);
	m[ 3][ 2] = - vView.Dot( vFrom);
	m[ 3][ 3] = 1.0f;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetProjectionMatrix(float fov,float aspect,float nearplane,float farplane)
{
	fov /= 2;

	float w = aspect * ( COS( fov) / SIN( fov));
	float h = 1.0f * ( COS( fov) / SIN( fov));
	float q = farplane / (farplane - nearplane);

	Zero();

	m[ 0][ 0] = w;
	m[ 1][ 1] = h;
	m[ 2][ 2] = q;
	m[ 2][ 3] = 1.0f;
	m[ 3][ 2] = -q * nearplane;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetProjectionMatrix_Orthgonal(float w,float h,float nearplane,float farplane)
{
	Identity();

	m[ 0][ 0] = 2.0f / w;
	m[ 1][ 1] = 2.0f / h;
	m[ 2][ 2] = 1.0f / (farplane - nearplane);

    /*
     * The projection matrix scales (xmin,ymin,zmin)-(xmax,ymax,zmax) to
     * (-1,-1,0)-(1,1,1).  The window scale will then scale this to the
     * buffer.
     * 
     * P = [     2 / xdiff             0                   0           0 ]
     *     [       0                 2 / ydiff             0           0 ]
     *     [       0                   0                 1 / zdiff     0 ]
     *     [ 1 - 2*xmax / xdiff  1 - 2*ymax / ydiff  1 - zmax / zdiff) 1 ]
     */
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::Matrix4x4::SetMirrorMatrix(const XiahGameEngine::Plane3 &plane)
{
	float fX = plane.n.x;
	float fY = plane.n.y;
	float fZ = plane.n.z;
	float fD = plane.d;

	*this = Matrix4x4((0.0f - (2.0f * fX * fX)), (0.0f - (2.0f * fX * fY)), (0.0f - (2.0f * fX * fZ)), 0.0f,
					 (0.0f - (2.0f * fY * fX)), (0.0f - (2.0f * fY * fY)), (0.0f - (2.0f * fY * fZ)), 0.0f,
					 (0.0f - (2.0f * fZ * fX)), (0.0f - (2.0f * fZ * fY)), (0.0f - (2.0f * fZ * fZ)), 0.0f,
					 (0.0f - (2.0f * fD * fX)), (0.0f - (2.0f * fD * fY)), (0.0f - (2.0f * fD * fZ)), 1.0f);
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::Matrix4x4::GetVM_View()
{
	return Vector3( _13, _23, _33);
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::Matrix4x4::GetVM_Right()
{
	return Vector3( _11, _21, _31);
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::Matrix4x4::GetVM_Up()
{
	return Vector3( _12, _22, _32);
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::operator *(XiahGameEngine::Vector3 &v,XiahGameEngine::Matrix4x4 &m)
{
	XiahGameEngine::Vector3 rV;

	rV.x = v.x * m.m[ 0][ 0] + v.y * m.m[ 1][ 0] + v.z * m.m[ 2][ 0] + m.m[ 3][ 0];
	rV.y = v.x * m.m[ 0][ 1] + v.y * m.m[ 1][ 1] + v.z * m.m[ 2][ 1] + m.m[ 3][ 1];
	rV.z = v.x * m.m[ 0][ 2] + v.y * m.m[ 1][ 2] + v.z * m.m[ 2][ 2] + m.m[ 3][ 2];

	return rV;
}

//---------------------------------------------------------------------------------------
inline void XiahGameEngine::operator *=(Vector3 &v,Matrix4x4 &m)
{
	XiahGameEngine::Vector3 rV;

	rV.x = v.x * m.m[ 0][ 0] + v.y * m.m[ 1][ 0] + v.z * m.m[ 2][ 0] + m.m[ 3][ 0];
	rV.y = v.x * m.m[ 0][ 1] + v.y * m.m[ 1][ 1] + v.z * m.m[ 2][ 1] + m.m[ 3][ 1];
	rV.z = v.x * m.m[ 0][ 2] + v.y * m.m[ 1][ 2] + v.z * m.m[ 2][ 2] + m.m[ 3][ 2];
	
	v = rV;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::WorldToScreen(Vector3 src,float width,float height,float nearZ,float farZ,Matrix4x4 &ProjectionMatrix,Matrix4x4 &ViewMatrix)
{
	XiahGameEngine::Vector3 result;

	result = src * ViewMatrix;
	
	float w = ProjectionMatrix.m[ 0][ 0];
	float h = ProjectionMatrix.m[ 1][ 1];
	//float q = ProjectionMatrix.m[ 2][ 2];

	if( ProjectionMatrix.m[ 2][ 3] != 0)
	{
		result.x *= w;
		result.y *= h;

		result.x /= result.z;
		result.y /= result.z;
	}
	else
	{
		result.x *= w;
		result.y *= h;
	}

	result.x *= width / 2;

	result.y = -result.y;
	result.y *= height / 2;

	result.x += width / 2;
	result.y += height / 2;


	return result;
}

//---------------------------------------------------------------------------------------
inline XiahGameEngine::Vector3 XiahGameEngine::ScreenToWorld(Vector3 src,float width,float height,float nearZ,float farZ,Matrix4x4 &ProjectionMatrix,Matrix4x4 &ViewMatrix)
{
	XiahGameEngine::Vector3 result;
	
	src.x -= width / 2;
	src.y -= height / 2;
	src.x /= width / 2;
	src.y /= height / 2;

	src.y = -src.y;

	XiahGameEngine::Matrix4x4 invView = ViewMatrix.GetInverse();

	float w = ProjectionMatrix.m[ 0][ 0];
	float h = ProjectionMatrix.m[ 1][ 1];
	float q = ProjectionMatrix.m[ 2][ 2];

	if( ProjectionMatrix.m[ 2][ 3] != 0) // Perspective 이면
	{
		float z = q * ( nearZ + (farZ - nearZ) * src.z) - q * nearZ;

		src.x *= z;
		src.y *= z;

		src.x /= w;
		src.y /= h;

		src.z = z;

		result = src * invView;
	}
	else
	{
		src.x /= w;
		src.y /= h;

//		float z = q * ( nearZ + (farZ - nearZ) * src.z) - q * nearZ;
		src.z = nearZ + (farZ - nearZ) * src.z;
//		src.z = z;

		result = src * invView;
	}
	
	return result;
}
