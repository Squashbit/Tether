#pragma once

#include <Tether/Math/Defs.hpp>

#include <cstdint>
#include <cmath>

namespace Tether::Math
{
	// Vector2 must be of a numeric type
	template<typename T>
	class Vector2
	{
	public:
		Vector2();
		Vector2(T value);
		Vector2(T x, T y);

		Vector2(const Vector2& ref);
		Vector2<T> operator=(const Vector2<T>& toSet);

		T& operator[](int index);

		// Arithmetic (+ - * /)
		Vector2<T> operator+(const Vector2<T>& value);
		Vector2<T> operator-(const Vector2<T>& value);
		Vector2<T> operator*(const Vector2<T>& value);
		Vector2<T> operator/(const Vector2<T>& value);
		Vector2<T> operator+(const T& value);
		Vector2<T> operator-(const T& value);
		Vector2<T> operator*(const T& value);
		Vector2<T> operator/(const T& value);

		// Assignment (+= -= *= /=)
		Vector2<T>& operator+=(const Vector2<T>& value);
		Vector2<T>& operator-=(const Vector2<T>& value);
		Vector2<T>& operator*=(const Vector2<T>& value);
		Vector2<T>& operator/=(const Vector2<T>& value);
		Vector2<T>& operator+=(const T& value);
		Vector2<T>& operator-=(const T& value);
		Vector2<T>& operator*=(const T& value);
		Vector2<T>& operator/=(const T& value);

		T x = static_cast<T>(0);
		T y = static_cast<T>(0);

		static Vector2<T> Normalize(Vector2<T> v);
		static float Dot(Vector2<T> v1, Vector2<T> v2);
		static float Length(Vector2<T> v);
	};
}

#include <Tether/Math/Vector2.inl>
