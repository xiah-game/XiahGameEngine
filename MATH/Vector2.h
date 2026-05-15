#pragma once

//---------------------------------------------------------------------------------------
// 2차원 Vector
namespace XiahGameEngine
{
	struct XIAHGE_API Vector2	// 어차피 C++이니까. 굳이 class로 할 이유가 없겠지.. 깔끔한게 최고다.
	{
	public:
		union
		{
			float xy[ 2];
			float uv[ 2];
			struct { float x, y;};
			struct { float u, v;};
		};

	public:
		inline Vector2();
		inline Vector2(const Vector2 &v);
		inline Vector2(float x,float y);

		inline float &operator [] (int iAxis);
		inline bool operator < (const Vector2 &v) const;
		inline bool operator > (const Vector2 &v) const;
		inline bool operator == (const Vector2 &v) const;
		inline bool operator != (const Vector2 &v) const;

		inline operator float *();
		inline operator const float *() const;
		
		inline Vector2 operator = (const Vector2 &v);
		inline void operator -= (const Vector2 &v);
		inline void operator += (const Vector2 &v);
		inline void operator /= (float fScalar);
		inline void operator *= (float fScalar);

		inline Vector2 operator - (const Vector2 &v) const;
		inline Vector2 operator + (const Vector2 &v) const;
		inline Vector2 operator - (const Vector2 &v);
		inline Vector2 operator + (const Vector2 &v);
		inline Vector2 operator / (float fScalar) const;
		inline Vector2 operator * (float fScalar) const;
		inline Vector2 operator / (float fScalar);
		inline Vector2 operator * (float fScalar);

		inline void SetLength(float fLength);
		inline float GetLengthSqr() const;
		inline float GetLengthSqr();
		inline float GetLength() const;
		inline float GetLength();

		inline float GetAngle(const Vector2 &v);
		inline float Distance(const Vector2 &v) const;
		inline float Distance(const Vector2 &v);
		inline float Dot(const Vector2 &v) const;
		inline float Dot(const Vector2 &v);

		inline Vector2 Lerp(const Vector2 &v,float fFactor) const;
		inline Vector2 Lerp(const Vector2 &v,float fFactor);
		inline Vector2 GetNormal() const;
		inline Vector2 GetNormal();
		inline void Normalize();
	};

	inline Vector2 operator - (const Vector2 &v);
	inline Vector2 operator - (Vector2 &v);
	inline Vector2 operator * (float scalar,Vector2 &v);
};