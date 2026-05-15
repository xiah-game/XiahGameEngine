#pragma once

//---------------------------------------------------------------------------------------
// 3차원 평면
namespace XiahGameEngine
{
	struct XIAHGE_API Plane3
	{
	public:
		union
		{
			struct
			{
				float a,b,c,d;
			};
			
			struct {
				Vector3 n;
				float d;
			};
		};

	public:
		inline Plane3();
		inline Plane3(const Plane3 &p);
		inline Plane3(float fA,float fB,float fC,float fD);
		inline Plane3(const Vector3 &vNormal,float fDistance);
		inline Plane3(const Vector3 &a,const Vector3 &b,const Vector3 &c);

		inline Plane3 &operator = (const Plane3 &p);

		inline Vector3 Normalize(const Vector3 &a,const Vector3 &b,const Vector3 &c);

		inline float GetDistance(const Vector3 &v) const;
		inline float GetDistance(const Vector3 &v);

		inline bool Intersect(const Vector3 &vStart,const Vector3 &vEnd);
		inline bool Intersect(const Vector3 &vStart,const Vector3 &vEnd,Vector3 &vCollide);

		inline bool IsFront(const Vector3 &v) const;
	};
};