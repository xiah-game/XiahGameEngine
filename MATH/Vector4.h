#pragma once

//---------------------------------------------------------------------------------------
// 4Â÷¿ø Vetor
namespace XiahGameEngine
{
	struct XIAHGE_API Vector4
	{
	public:
		union
		{
			float xyzw[ 4];
			float rgba[ 4];
			struct { float x, y, z, w;};
			struct { float r, g, b, a;};
		};

	public:
		inline Vector4();
		inline Vector4(const Vector4 &v);
		inline Vector4(float fX,float fY,float fZ,float fW);

		inline float &operator [] (int iAxis);
		inline bool operator < (const Vector4 &v) const;
		inline bool operator > (const Vector4 &v) const;
		inline bool operator == (const Vector4 &v) const;
		inline bool operator != (const Vector4 &v) const;

		inline operator float *();
		inline operator const float *() const;

		inline Vector4 operator = (const Vector4 &v);
		inline void operator -= (const Vector4 &v);
		inline void operator += (const Vector4 &v);
		inline void operator /= (float fScalar);
		inline void operator *= (float fScalar);

		inline Vector4 operator - (const Vector4 &v) const;
		inline Vector4 operator + (const Vector4 &v) const;
		inline Vector4 operator - (const Vector4 &v);
		inline Vector4 operator + (const Vector4 &v);
		inline Vector4 operator / (float fScalar) const;
		inline Vector4 operator * (float fScalar) const;
		inline Vector4 operator / (float fScalar);
		inline Vector4 operator * (float fScalar);

		inline void SetLength(float fLength);
		inline float GetLengthSqr() const;
		inline float GetLengthSqr();
		inline float GetLength() const;
		inline float GetLength();

		inline float GetAngle(const Vector4 &v);
		inline float Dot(const Vector4 &v) const;
		inline float Dot(const Vector4 &v);
		inline float Distance(const Vector4 &v) const;
		inline float Distance(const Vector4 &v);
		
		inline Vector4 Lerp(const Vector4 &v, float fFactor) const;
		inline Vector4 Lerp(const Vector4 &v, float fFactor);
		inline Vector4 GetNormal() const;
		inline Vector4 GetNormal();
		inline void Normalize();
	};


	inline Vector4 operator - (const Vector4 &v);
	inline Vector4 operator - (Vector4 &v);
	inline Vector4 operator * (float scalar,Vector4 &v);
};