#pragma once

//---------------------------------------------------------------------------------------
// Quaternion°´Ã¼ Vector4¶û Â«»Í ½ÃÅ³±î °í¹ÎÁß
namespace XiahGameEngine
{
	struct XIAHGE_API Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

	public:
		inline Quaternion();
		inline Quaternion(const Quaternion &q);
		inline Quaternion(const float fX,float fY,float fZ,float fW);
		inline Quaternion(const Vector3 &vAxis,float Angle);
		inline Quaternion(const Vector3 &vEuler);

		inline operator float * (void);

		inline void operator *= (const Quaternion &q);
		inline Quaternion &operator = (const Quaternion &q);
		inline void operator -= (const Quaternion &q);
		inline void operator += (const Quaternion &q);
		inline void operator /= (float fScalar);
		inline void operator *= (float fScalar);

		inline Quaternion operator * (const Quaternion &q);
		inline Quaternion operator - (const Quaternion &q);
		inline Quaternion operator + (const Quaternion &a);
		inline Quaternion operator / (float fScalar);
		inline Quaternion operator * (float fScalar);

		inline void Identity();
		inline void SetEuler(const Vector3 &v);
		inline void SetAxisAngle(const Vector3 &vAxis,float fAngle);
		
		inline float Dot(const Quaternion &q);
		
		inline float GetLengthSqr();
		inline float GetLength();

		inline Quaternion GetNormal();
		inline void Normalize();

		inline Quaternion Slerp(const Quaternion &q,float fFactor);
		inline Quaternion Lerp(const Quaternion &q,float fFactor);
	};
};