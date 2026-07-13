#include "stdafx.h"

#include "Quaternion.h"
#include "vector_math.h"


Quaternion Quaternion::Identity()
{
	return { 1.0f, 0.0f, 0.0f, 0.0f };
}


Quaternion Quaternion::Conjugate() const
{
	return { w, -x, -y, -z };
}


float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
	return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

Quaternion Quaternion::Inverse() const
{
	float len2 = w*w + x*x + y*y + z*z;

	Quaternion q = Conjugate();

	if (len2 > 0.0f)
	{
		float inv = 1.0f / len2;

		q.w *= inv;
		q.x *= inv;
		q.y *= inv;
		q.z *= inv;
	}

	return q;
}


Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
	return	{	w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z,
				w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y,
				w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x,
				w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w
			};
}


Quaternion Quaternion::FromAxisAngle(float ax, float ay, float az, float angle)
{
	float len = std::sqrt(ax*ax + ay*ay + az*az);

	if (len == 0.0f)
		return Identity();

	float inv = 1.0f / len;

	ax *= inv;
	ay *= inv;
	az *= inv;

	float half = angle * 0.5f;
	float s = std::sin(half);

	return { std::cos(half), ax*s, ay*s, az*s };
}


void Quaternion::ToAxisAngle( float& ax, float& ay, float& az, float& angle) const
{
	Quaternion q = *this;
	q.Normalize();

	angle = 2.0f * std::acos(q.w);

	float s = std::sqrt(1.0f - q.w * q.w);

	if (s < 1e-6f)
	{
		ax = 1.0f;
		ay = 0.0f;
		az = 0.0f;
	}
	else
	{
		ax = q.x / s;
		ay = q.y / s;
		az = q.z / s;
	}
}


void Quaternion::Normalize()
{
	float len = std::sqrt(w*w + x*x + y*y + z*z);

	if (len > 0.0f)
	{
		w /= len;
		x /= len;
		y /= len;
		z /= len;
	}
}


Quaternion Quaternion::FromMatrix(const Matr4& m)
{
	Quaternion q;

	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if (trace > 0.0f)
	{
		float s = std::sqrt(trace + 1.0f) * 2.0f;

		q.w = 0.25f * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
	}
	else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2])
	{
		float s = std::sqrt(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;

		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	}
	else if (m.m[1][1] > m.m[2][2])
	{
		float s = std::sqrt(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;

		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	}
	else
	{
		float s = std::sqrt(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;

		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25f * s;
	}

	q.Normalize();
	return q;
}


Matr4 Quaternion::ToMatrix() const
{
	Quaternion q = *this;
	q.Normalize();

	Matr4 m;

	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;

	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	m.m[0][0] = 1.0f - 2.0f * (yy + zz);
	m.m[0][1] =        2.0f * (xy - wz);
	m.m[0][2] =        2.0f * (xz + wy);
	m.m[0][3] = 0.0;

	m.m[1][0] =        2.0f * (xy + wz);
	m.m[1][1] = 1.0f - 2.0f * (xx + zz);
	m.m[1][2] =        2.0f * (yz - wx);
	m.m[1][3] = 0.0;

	m.m[2][0] =        2.0f * (xz - wy);
	m.m[2][1] =        2.0f * (yz + wx);
	m.m[2][2] = 1.0f - 2.0f * (xx + yy);
	m.m[2][3] = 0.0;

	m.m[3][0] = 0.0;
	m.m[3][1] = 0.0;
	m.m[3][2] = 0.0;
	m.m[3][3] = 1.0;

	return m;
}


Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
	Quaternion q1 = a;
	Quaternion q2 = b;

	float dot = Dot(q1, q2);

	// Use shortest path
	if (dot < 0.0f)
	{
		dot = -dot;

		q2.w = -q2.w;
		q2.x = -q2.x;
		q2.y = -q2.y;
		q2.z = -q2.z;
	}

	// Very close -> linear interpolation
	if (dot > 0.9995f)
	{
		Quaternion r;

		r.w = q1.w + t * (q2.w - q1.w);
		r.x = q1.x + t * (q2.x - q1.x);
		r.y = q1.y + t * (q2.y - q1.y);
		r.z = q1.z + t * (q2.z - q1.z);

		r.Normalize();
		return r;
	}

	float theta0 = std::acos(dot);
	float theta  = theta0 * t;

	float sinTheta  = std::sin(theta);
	float sinTheta0 = std::sin(theta0);

	float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
	float s1 = sinTheta / sinTheta0;

	Quaternion r;

	r.w = s0*q1.w + s1*q2.w;
	r.x = s0*q1.x + s1*q2.x;
	r.y = s0*q1.y + s1*q2.y;
	r.z = s0*q1.z + s1*q2.z;

	return r;
}
