#pragma once

//---------------------------------------------------------------------------------------
// View Frustum°´Ã¼

namespace XiahGameEngine
{
	struct XIAHGE_API Frustum
	{
	public:
		Plane3 m_Frustum[ 6];

	public:
		inline Frustum();
		inline Frustum(const Matrix4x4 &m);

		inline void Set(const Matrix4x4 &m);

		inline bool Visible(const Vector3 &pos) const;
		inline bool Visible(const Sphere3 &sphere) const;
		inline bool Visible(const BBoxAABB3 &bbox) const;
		inline bool Visible(const BBoxOBB3 &box) const;
	};
};