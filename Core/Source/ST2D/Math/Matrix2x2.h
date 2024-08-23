#pragma once
#include "Vector2.h"
namespace ST
{
    struct ST_API Matrix2x2
    {
	    Matrix2x2();
        Matrix2x2(const real& radian);
        Matrix2x2(const Matrix2x2& mat);
        Matrix2x2(const Vector2& col1, const Vector2& col2);
        Matrix2x2(const real& col1_x, 
            const real& col1_y, const real& col2_x, const real& col2_y);
        Matrix2x2(Matrix2x2&& other) = default;

        Matrix2x2& operator=(const Matrix2x2& rhs);
        Matrix2x2& operator+=(const Matrix2x2& rhs);
        Matrix2x2& operator-=(const Matrix2x2& rhs);
        Matrix2x2& operator*=(const real& factor);
        Matrix2x2& operator/=(const real& factor);

        bool operator==(const Matrix2x2& rhs)const;

        real& operator[](const int& index);
        real operator[](const int& index)const;

        Matrix2x2 operator+(const Matrix2x2& rhs)const;
        Matrix2x2 operator-(const Matrix2x2& rhs)const;

        Matrix2x2 operator*(const real& factor) const;

        Vector2 row1()const;
        Vector2 row2()const;

        real determinant()const;
        Matrix2x2& transpose();
        Matrix2x2& invert();
        Matrix2x2& multiply(const Matrix2x2& rhs);
        Vector2 multiply(const Vector2& rhs)const;

        Matrix2x2& clear();
        Matrix2x2& set(const real& col1_x, const real& col1_y, const real& col2_x, const real& col2_y);
        Matrix2x2& set(const Vector2& col1, const Vector2& col2);
        Matrix2x2& set(const Matrix2x2& other);
        Matrix2x2& set(const real& radian);

        static Matrix2x2 fromRadian(const real& radians);
        static Matrix2x2 skewSymmetricMatrix(const Vector2& r);
        static Matrix2x2 identityMatrix();
        static Vector2 multiply(const Matrix2x2& lhs, const Vector2& rhs);
        static Matrix2x2 multiply(const Matrix2x2& lhs, const Matrix2x2& rhs);
        static real determinant(const Matrix2x2& mat);
        static bool invert(Matrix2x2& mat);

        union 
        {
			struct
			{
				real m11, m21, m12, m22;
			};
            real data[4];
            Vector2 columns[2];
        };

    };
}
