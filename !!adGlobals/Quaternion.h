#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector_math.h"


struct Quaternion
{
	float w, x, y, z;

	static Quaternion Identity();
	static Quaternion FromAxisAngle(float ax, float ay, float az, float angle);
	static Quaternion FromMatrix(const Matr4& m);
	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
	static float      Dot(const Quaternion& a, const Quaternion& b);

	Quaternion Conjugate() const;
	Quaternion Inverse() const;
	Matr4      ToMatrix() const;
	void       ToAxisAngle(float& ax, float& ay, float& az, float& angle) const;
	Quaternion operator*(const Quaternion& rhs) const;							// combines two rotations

	void Normalize();
};

struct TRSTransform
{
	Vec3 vTranslation;
	Vec3 vScale;
	Quaternion qRotation;
};


Matr4        Mat4Compose(const TRSTransform& tc);
Matr4        Mat4Interpolate(const Matr4& A, const Matr4& B, float t);
TRSTransform TRSTransformInterpolate(const TRSTransform& a, const TRSTransform& b, float t);
TRSTransform Mat4Decompose(const Matr4& m);

#endif