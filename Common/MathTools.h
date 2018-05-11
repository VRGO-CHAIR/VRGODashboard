#pragma once
/********************************************************************************
MIT License

Copyright(c) 3dRudder 2017

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
********************************************************************************/

#include <cmath>


typedef double Mat_33[3][3];

inline float clamp(float x, float a, float b)
{

	return x < a ? a : (x > b ? b : x);
}

// Reset quaternion to identity
inline void Identity(vr::HmdQuaternion_t& quat)
{
	quat.w = 1;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;
}

// multiplication of quaternions
inline vr::HmdQuaternion_t operator*(const vr::HmdQuaternion_t& a, const vr::HmdQuaternion_t& b)
{
	vr::HmdQuaternion_t tmp;

	tmp.w = (b.w * a.w) - (b.x * a.x) - (b.y * a.y) - (b.z * a.z);
	tmp.x = (b.w * a.x) + (b.x * a.w) + (b.y * a.z) - (b.z * a.y);
	tmp.y = (b.w * a.y) + (b.y * a.w) + (b.z * a.x) - (b.x * a.z);
	tmp.z = (b.w * a.z) + (b.z * a.w) + (b.x * a.y) - (b.y * a.x);

	return tmp;
}

// Inverse quaternion
inline void Inverse(vr::HmdQuaternion_t& q)
{
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
}

// Quaternion to matrix 3x3
inline void QuatToMat(const vr::HmdQuaternion_t& quat, Mat_33 &mat)
{
	mat[0][0] = 1.0 - 2.0 * quat.y * quat.y - 2.0 * quat.z * quat.z;
	mat[0][1] = 2.0 * quat.x * quat.y - 2.0 * quat.z * quat.w;
	mat[0][2] = 2.0 * quat.x * quat.z + 2.0 * quat.y * quat.w;

	mat[1][0] = 2.0 * quat.x * quat.y + 2.0 * quat.z * quat.w;
	mat[1][1] = 1.0 - 2.0 * quat.x * quat.x - 2.0 * quat.z * quat.z;
	mat[1][2] = 2.0 * quat.y * quat.z - 2.0 * quat.x * quat.w;

	mat[2][0] = 2.0 * quat.x * quat.z - 2.0 * quat.y * quat.w;
	mat[2][1] = 2.0 * quat.y * quat.z + 2.0 * quat.x * quat.w;
	mat[2][2] = 1.0 - 2.0 * quat.x * quat.x - 2.0 * quat.y * quat.y;
}

// Matrix 3x3 to quaternion
inline void MatToQuat(const Mat_33 &mat, vr::HmdQuaternion_t& quat)
{
	quat.w = sqrt(1 + mat[0][0] + mat[1][1] + mat[2][2]) / 2.0;
	double w4 = (4.0 * quat.w);
	if (w4 == 0)
	{
		quat.x = quat.y = quat.z = 0;
	}
	else
	{
		quat.x = (mat[2][1] - mat[1][2]) / (w4);
		quat.y = (mat[0][2] - mat[2][0]) / (w4);
		quat.z = (mat[1][0] - mat[0][1]) / (w4);
	}
}

// Vector product u ^ v
inline vr::HmdVector3d_t VectorProd(const vr::HmdVector3d_t& u, const vr::HmdVector3d_t& v)
{
	vr::HmdVector3d_t res;
	res.v[0] = u.v[1] * v.v[2] - u.v[2] * v.v[1];
	res.v[1] = u.v[2] * v.v[0] - u.v[0] * v.v[2];
	res.v[2] = u.v[0] * v.v[1] - u.v[1] * v.v[0];
	return res;
}

// Multiplication vector value u * a
inline vr::HmdVector3d_t MultVector(const vr::HmdVector3d_t& u, double ratio)
{
	vr::HmdVector3d_t res;
	for (int i = 0; i < 3; ++i)
		res.v[i] = ratio * u.v[i];
	return res;
}

// Sum vector u + v
inline vr::HmdVector3d_t SumVector(const vr::HmdVector3d_t& u, const vr::HmdVector3d_t& v)
{
	vr::HmdVector3d_t res;
	for (int i = 0; i < 3; ++i)
		res.v[i] = u.v[i] + v.v[i];
	return res;
}

// Scalar product u.v
inline double ScalProd(vr::HmdVector3d_t &u, vr::HmdVector3d_t &v)
{
	return u.v[0] * v.v[0] + u.v[1] * v.v[1] + u.v[2] * v.v[2];
}

// Rotate vector around an axis by an angle in radian (Rodrigues' formula)
inline vr::HmdVector3d_t RotateVector(vr::HmdVector3d_t &vect, vr::HmdVector3d_t &axis, double angle_rad)
{
	vr::HmdVector3d_t res;
	vr::HmdVector3d_t tmp;
	double ratio;
	double cos_angle = cos(angle_rad);
	double sin_angle = sin(angle_rad);

	// V' = V*cos(angle)
	res = MultVector(vect, cos_angle);

	// V' = V' + sin(angle) * (axis ^ v)
	tmp = VectorProd(axis, vect);
	tmp = MultVector(tmp, sin_angle);
	res = SumVector(res, tmp);

	// V' = V' + (axis * v) * (1 - cos(angle)) * axis
	ratio = ScalProd(axis, vect) * (1.0 - cos_angle);
	tmp = MultVector(axis, ratio);
	res = SumVector(res, tmp);

	return(res);
}

// Split a matrix in 3 axis vector
inline void MatToVectors(Mat_33 mat_init, vr::HmdVector3d_t &vect_x, vr::HmdVector3d_t &vect_y, vr::HmdVector3d_t &vect_z)
{
	int ii;

	// Get vector coordinates
	for (ii = 0; ii < 3; ii++)
		vect_x.v[ii] = mat_init[ii][0];
	for (ii = 0; ii < 3; ii++)
		vect_y.v[ii] = mat_init[ii][1];
	for (ii = 0; ii < 3; ii++)
		vect_z.v[ii] = mat_init[ii][2];
}

// Split a quaternion in 3 axis vector
inline void QuatToVectors(vr::HmdQuaternion_t quat_init, vr::HmdVector3d_t &vect_x, vr::HmdVector3d_t &vect_y, vr::HmdVector3d_t &vect_z)
{
	Mat_33 mat_init;

	QuatToMat(quat_init, mat_init);

	// Get vector coordinates
	MatToVectors(mat_init, vect_x, vect_y, vect_z);
}

// Create a quaternion with an angle in radian and an normalized axis
inline vr::HmdQuaternion_t FromAngleAxis(double angle, const vr::HmdVector3d_t& axis)
{
	vr::HmdQuaternion_t q;
	const double dHalfAngle = 0.5 * angle;
	const double dSin = sin(dHalfAngle);
	q.w = cos(dHalfAngle);
	q.x = dSin * axis.v[0];
	q.y = dSin * axis.v[1];
	q.z = dSin * axis.v[2];
	return q;
}

inline vr::HmdVector3d_t RotateVect(const Mat_33& mat, vr::HmdVector3d_t& vect)
{
	vr::HmdVector3d_t tmp;
	for (int i = 0; i < 3; ++i)
		tmp.v[i] = vect.v[0] * mat[i][0] + vect.v[1] * mat[i][1] + vect.v[2] * mat[i][2];
	return tmp;
}

inline void TransposeMat(const Mat_33& mat, Mat_33& transposedMat)
{
	int i, j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			transposedMat[j][i] = mat[i][j];
}

// Rotate a vector by a quaternion
inline void Transform(const vr::HmdQuaternion_t& rotation, vr::HmdVector3d_t &value)
{
	double num12 = rotation.x + rotation.x;
	double num2 = rotation.y + rotation.y;
	double num = rotation.z + rotation.z;
	double num11 = rotation.w * num12;
	double num10 = rotation.w * num2;
	double num9 = rotation.w * num;
	double num8 = rotation.x * num12;
	double num7 = rotation.x * num2;
	double num6 = rotation.x * num;
	double num5 = rotation.y * num2;
	double num4 = rotation.y * num;
	double num3 = rotation.z * num;
	double num15 = ((value.v[0] * ((1.0 - num5) - num3)) + (value.v[1] * (num7 - num9))) + (value.v[2] * (num6 + num10));
	double num14 = ((value.v[0] * (num7 + num9)) + (value.v[1] * ((1.0 - num8) - num3))) + (value.v[2] * (num4 - num11));
	double num13 = ((value.v[0] * (num6 - num10)) + (value.v[1] * (num4 + num11))) + (value.v[2] * ((1.0 - num8) - num5));
	value.v[0] = num15;
	value.v[1] = num14;
	value.v[2] = num13;
}

inline void MapToCircle(float& xSquare, float& ySquare)
{
	float xCircle = xSquare * sqrt(1.f - (0.5f * pow(ySquare, 2.f)));
	float yCircle = ySquare * sqrt(1.f - (0.5f * pow(xSquare, 2.f)));
	xSquare = xCircle;
	ySquare = yCircle;
}