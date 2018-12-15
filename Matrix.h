#pragma once

#include "vector.h"

/*
	A class to support low level matrix operations (e.g. matrix multiplication).
*/
class Matrix
{
public:
	float data[4][4];

	Matrix()
	{
		data[0][0] = 1.0f;
		data[0][1] = 0.0f;
		data[0][2] = 0.0f;
		data[0][3] = 0.0f;
		data[1][0] = 0.0f;
		data[1][1] = 1.0f;
		data[1][2] = 0.0f;
		data[1][3] = 0.0f;
		data[2][0] = 0.0f;
		data[2][1] = 0.0f;
		data[2][2] = 1.0f;
		data[2][3] = 0.0f;
		data[3][0] = 0.0f;
		data[3][1] = 0.0f;
		data[3][2] = 0.0f;
		data[3][3] = 1.0f;
	};

	Matrix(float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p)
	{
		data[0][0] = a;
		data[0][1] = b;
		data[0][2] = c;
		data[0][3] = d;
		data[1][0] = e;
		data[1][1] = f;
		data[1][2] = g;
		data[1][3] = h;
		data[2][0] = i;
		data[2][1] = j;
		data[2][2] = k;
		data[2][3] = l;
		data[3][0] = m;
		data[3][1] = n;
		data[3][2] = o;
		data[3][3] = p;
	};

	Matrix(float data[4][4])
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; j++)
			{
				this->data[i][j] = data[i][j];
			}
		}
	}

	inline Vector operator*(Vector const& right)
	{
		float vectorValues[4];
		float newValues[4];
		vectorValues[0] = right.x;
		vectorValues[1] = right.y;
		vectorValues[2] = right.z;
		vectorValues[3] = 1;

		for (int i = 0; i < 4; i++)
		{
			newValues[i] = 0;
			for (int j = 0; j < 4; j++)
			{
				newValues[i] = newValues[i] + (vectorValues[i] * this->operator[](i)[j]);
			}
		}

		return Vector(newValues[0], newValues[1], newValues[2]);
	}

	inline Matrix operator*(Matrix const& right);

	inline const float* operator[](int i) const
	{
		return data[i];
	}
};

inline Matrix Matrix::operator*(Matrix const& right)
{
	float newData[4][4];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			float val1 = this->operator[](i)[0] * right[0][j];
			float val2 = this->operator[](i)[1] * right[1][j];
			float val3 = this->operator[](i)[2] * right[2][j];
			float val4 = this->operator[](i)[3] * right[3][j];
			newData[i][j] = val1 + val2 + val3 + val4;
		}
	}
	return Matrix(newData);
}


