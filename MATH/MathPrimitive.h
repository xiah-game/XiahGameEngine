#pragma once

//---------------------------------------------------------------------------------------
// 기하학 물체 용 구조체
//---------------------------------------------------------------------------------------

namespace XiahGameEngine
{
	struct XIAHGE_API BBoxAABB3;

	//---------------------------------------------------------------------------------------
	// 3차원 구
	struct XIAHGE_API Sphere3
	{
	public:
		Vector3 m_vOrigin;
		float   m_fRadius;

	public:
		inline Sphere3();
		inline Sphere3(const Vector3 &vOrigin,float fRadius);

		inline bool Intersect(const BBoxAABB3 &BBox) const;
		inline bool Intersect(const Sphere3 &sphere) const;
	};

	//---------------------------------------------------------------------------------------
	// AABB Bounding Box
	struct XIAHGE_API BBoxAABB3
	{
	public:
		Vector3 m_vMin;
		Vector3 m_vMax;

	public:
		inline BBoxAABB3();
		inline BBoxAABB3(Vector3 vMin,Vector3 vMax);

		inline Vector3 Center();
		inline Vector3 Size();

		inline bool Intersect(const BBoxAABB3 &bbox) const;
		inline bool Intersect(const Sphere3 &sphere) const;

		inline BBoxAABB3 operator + (const BBoxAABB3 &bbox);
		inline void operator += (const BBoxAABB3 &bbox);
	};

	//---------------------------------------------------------------------------------------
	struct XIAHGE_API BBoxOBB3
	{
	public:
		Vector3 m_Pos;
		Vector3 m_Size;
		Matrix4x4 m_Orient;
		BBoxAABB3 m_BBoxAABB;

	public:
		inline BBoxOBB3();
		inline BBoxOBB3(const Vector3& pos,const Vector3& size,const Matrix4x4& orient);
		inline BBoxOBB3(const BBoxOBB3& box);
		inline BBoxOBB3(BBoxAABB3& box,Matrix4x4& orient);

		inline void SetPosition(const Vector3 &pos);
		inline void SetSize(const Vector3 &size);
		inline void SetOrient(const Matrix4x4 &orient);

		inline Vector3 GetHeightMax();
	public:
		Vector3 m_HeightMax;
		Vector3 m_vCenter[ 6];
		Face3   m_Face[ 12];
		Plane3  m_Plane[ 6];
		Vector3 m_Point[ 8];
		Vector3 m_Center;

		inline void BuildInternalData();

	public:
		inline bool IsIntersect(const Vector3& start,const Vector3& end,Vector3* pStartCollidePoint);
	
		inline bool IsIntersect( BBoxAABB3* pAABB);
		inline bool IsIntersect( BBoxOBB3* pOBB);
	};
};