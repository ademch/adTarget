#ifndef QUATERNION_H
#define QUATERNION_H

struct Matr4;

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

#endif