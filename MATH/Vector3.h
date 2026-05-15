#pragma once

//---------------------------------------------------------------------------------------
// 3Â÷¿ø Vector
namespace XiahGameEngine
{
	struct XIAHGE_API Vector3
	{
	public:
		union
		{
			float xyz[ 3];
			struct { float x, y, z;};
		};

	public:
		inline Vector3();
		inline Vector3(const Vector3 &v);
		inline Vector3(float fX,float fY,float fZ);

		inline float &operator [] (int iAxis);
		inline bool operator < (const Vector3 &v) const;
		inline bool operator > (const Vector3 &v) const;
		inline bool operator == (const Vector3 &v) const;
		inline bool operator != (const Vector3 &v) const;

		inline operator float * ();
		inline operator const float *() const;

		inline Vector3 operator = (const Vector3 &v);
		inline void operator -= (const Vector3 &v);
		inline void operator += (const Vector3 &v);
		inline void operator /= (float fScalar);
		inline void operator *= (float fScalar);

		inline Vector3 operator - (const Vector3 &v) const;
		inline Vector3 operator + (const Vector3 &v) const;
		inline Vector3 operator - (const Vector3 &v);
		inline Vector3 operator + (const Vector3 &v);
		inline Vector3 operator / (float fScalar) const;
		inline Vector3 operator * (float fScalar) const;
		inline Vector3 operator / (float fScalar);
		inline Vector3 operator * (float fScalar);

		inline void SetLength(float fLength);
		inline float GetLengthSqr() const;
		inline float GetLengthSqr();
		inline float GetLength() const;
		inline float GetLength();

		inline float GetAngle(const Vector3 &v);

		inline float Dot(const Vector3 &v) const;
		inline float Dot(const Vector3 &v);

		inline Vector3 Cross(const Vector3 &v) const;
		inline Vector3 Cross(const Vector3 &v);
		inline float Distance(const Vector3 &v) const;
		inline float Distance(const Vector3 &v);

		inline Vector3 Lerp(const Vector3 &v, float fFactor) const;
		inline Vector3 Lerp(const Vector3 &v, float fFactor);
		inline Vector3 GetNormal() const;
		inline Vector3 GetNormal();
		inline void Normalize();

		inline Vector3 Min(const Vector3 &v);
		inline Vector3 Max(const Vector3 &v);
	};

	inline Vector3 operator - (const Vector3 &v);
	inline Vector3 operator - (Vector3 &v);
	inline Vector3 operator * (float scalar,Vector3 &v);
};