#pragma once

#include <Tether/Math/Vector2.hpp>

namespace Tether::Math
{
	// Vector3 must be of a numeric type
	template<typename T>
	class Vector3
	{
	public:
		Vector3();
		Vector3(T value);
		Vector3(T x, T y, T z);
		Vector3(Vector2<T> vec);

		Vector3(const Vector3& ref);
		Vector3<T> operator=(const Vector3<T>& toSet);

		T& operator[](int index);

		// Arithmetic (+ - * /)
		Vector3<T> operator+(const Vector3<T>& value) const;
		Vector3<T> operator-(const Vector3<T>& value) const;
		Vector3<T> operator*(const Vector3<T>& value) const;
		Vector3<T> operator/(const Vector3<T>& value) const;
		Vector3<T> operator+(const T& value) const;
		Vector3<T> operator-(const T& value) const;
		Vector3<T> operator*(const T& value) const;
		Vector3<T> operator/(const T& value) const;

		Vector3<T> operator-();

		// Assignment (+= -= *= /=)
		Vector3<T>& operator+=(const Vector3<T>& value);
		Vector3<T>& operator-=(const Vector3<T>& value);
		Vector3<T>& operator*=(const Vector3<T>& value);
		Vector3<T>& operator/=(const Vector3<T>& value);
		Vector3<T>& operator+=(const T& value);
		Vector3<T>& operator-=(const T& value);
		Vector3<T>& operator*=(const T& value);
		Vector3<T>& operator/=(const T& value);

		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;
	};
}

#include <Tether/Math/Vector3.inl>
