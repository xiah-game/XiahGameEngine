#pragma once

namespace XiahGameEngine
{
	struct XIAHGE_API Face3
	{
	public:
		Vector3 v[ 3];
		Plane3  plane;
	public:
		inline Face3();
		inline Face3(const Face3& face);
		inline Face3(const Vector3& a,const Vector3& b,const Vector3& c);
	
	public:
		inline bool IsIntersect(const Vector3& p1,const Vector3& p2);
		inline Vector3 GetIntersectPoint(const Vector3& p1,const Vector3& p2);
	};
};