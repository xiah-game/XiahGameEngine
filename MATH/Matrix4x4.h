#pragma once

//---------------------------------------------------------------------------------------
// 4x4За·Д 
//---------------------------------------------------------------------------------------

namespace XiahGameEngine
{
	struct XIAHGE_API Matrix4x4
	{
	public:
		union
		{
			float d[ 16];
			float m[ 4][ 4];

			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
		
			struct
			{
				Vector4 rx;
				Vector4 ry;
				Vector4 rz;

				union
				{
					struct
					{
						Vector3 t;
						float w;
					};

					struct
					{
						float x,y,z,w;
					};
				};
			};
		};

	public:
		inline Matrix4x4();
		inline Matrix4x4(float *pfData);
		inline Matrix4x4(float f11,float f12,float f13,float f14,
						float f21,float f22,float f23,float f24,
						float f31,float f32,float f33,float f34,
						float f41,float f42,float f43,float f44);
		inline Matrix4x4(const Matrix4x4 &m);

		inline Matrix4x4 &operator = (const Matrix4x4 &m);
		inline void operator *= (const Matrix4x4 &m);

		inline float operator () (int iRow,int iCol) const;
		inline float operator () (int iRow,int iCol);
		inline operator const float * () const;
		inline operator float * ();

		inline Matrix4x4 operator * (const Matrix4x4 &m);

		inline void Identity();
		inline void Zero();
		inline void SetRotationEuler(const Vector3 &v);
		inline void SetRotationAxisAngle(const Vector3 &v,float fAngle);
		inline void SetRotationQuaternion(const Quaternion &q);
		inline void SetRotationQuaternion_Flip(const Quaternion &q,bool bFlip);
		inline void SetRotationX(float fAngle);
		inline void SetRotationY(float fAngle);
		inline void SetRotationZ(float fAngle);
		inline void SetRotationTarget(const Vector3 &vOrigin,const Vector3 &vTarget);	// Direction

		inline void SetScale(const Vector3 &v);
		inline void SetScale(float fScale);
		inline Vector3 GetScale();
		
		inline void Transpose();
		inline float Determinant();
		inline Matrix4x4 GetInverse();
		inline void Inverse();

		inline void SetViewMatrix(Vector3 &vFrom,Vector3 &vAt,Vector3 &vUp);
		inline void SetProjectionMatrix(float fov,float aspect,float nearplane,float farplane);
		inline void SetProjectionMatrix_Orthgonal(float w,float h,float nearplane,float farplane);

		inline void SetMirrorMatrix(const Plane3 &plane);

		inline Vector3 GetVM_View();
		inline Vector3 GetVM_Right();
		inline Vector3 GetVM_Up();
	};

	inline Vector3 operator *(Vector3 &v,Matrix4x4 &m);

	inline void operator *=(Vector3 &v,Matrix4x4 &m);

	inline Vector3 WorldToScreen(Vector3 src,float width,float height,float nearZ,float farZ,Matrix4x4 &ProjectionMatrix,Matrix4x4 &ViewMatrix);
	inline Vector3 ScreenToWorld(Vector3 src,float width,float height,float nearZ,float farZ,Matrix4x4 &ProjectionMatrix,Matrix4x4 &ViewMatrix);
};

