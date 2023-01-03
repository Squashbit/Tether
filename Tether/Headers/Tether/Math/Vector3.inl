namespace Tether::Math
{
	template<typename T>
	TETHER_MATH_FUNC Vector3<T>::Vector3()
	{}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>::Vector3(T value)
		:
		x(value),
		y(value),
		z(value)
	{}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>::Vector3(T x, T y, T z)
		:
		x(x),
		y(y),
		z(z)
	{}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>::Vector3(Vector2<T> vec)
	{
		x = vec.x;
		y = vec.y;
	}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	TETHER_MATH_FUNC Vector3<T>::Vector3(const Vector3<T>& ref)
	{
		x = ref.x;
		y = ref.y;
		z = ref.z;
	}

	template<typename T>
	TETHER_MATH_FUNC T& Vector3<T>::operator[](int index)
	{
		switch (index)
		{
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}

		throw "Vector index out of range!";
		return x;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator=(const Vector3<T>& value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator+(const Vector3<T>& value) const
	{
		return Vector3<T>(*this) += value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator-(const Vector3<T>& value) const
	{
		return Vector3<T>(*this) -= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator*(const Vector3<T>& value) const
	{
		return Vector3<T>(*this) *= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator/(const Vector3<T>& value) const
	{
		return Vector3<T>(*this) /= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator+(const T& value) const
	{
		return Vector3<T>(*this) += value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator-(const T& value) const
	{
		return Vector3<T>(*this) -= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator*(const T& value) const
	{
		return Vector3<T>(*this) *= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator/(const T& value) const
	{
		return Vector3<T>(*this) /= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T> Vector3<T>::operator-()
	{
		return Vector3<T>(
			-x,
			-y,
			-z
		);
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& value)
	{
		x += value.x;
		y += value.y;
		z += value.z;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& value)
	{
		x -= value.x;
		y -= value.y;
		z -= value.z;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator*=(const Vector3<T>& value)
	{
		x *= value.x;
		y *= value.y;
		z *= value.z;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator/=(const Vector3<T>& value)
	{
		x /= value.x;
		y /= value.y;
		z /= value.z;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator+=(const T& value)
	{
		x += value;
		y += value;
		z += value;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator-=(const T& value)
	{
		x -= value;
		y -= value;
		z -= value;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator*=(const T& value)
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector3<T>& Vector3<T>::operator/=(const T& value)
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}
}