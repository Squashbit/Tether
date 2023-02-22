namespace Tether::Math
{
	template<typename T>
	Vector2<T>::Vector2()
	{}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>::Vector2(T value)
		:
		x(value),
		y(value)
	{}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>::Vector2(T x, T y)
		:
		x(x),
		y(y)
	{}

	// The ref is the one to copy from (I always forget that)
	template<typename T>
	TETHER_MATH_FUNC Vector2<T>::Vector2(const Vector2<T>& ref)
	{
		x = ref.x;
		y = ref.y;
	}

	template<typename T>
	TETHER_MATH_FUNC T& Vector2<T>::operator[](int index)
	{
		switch (index)
		{
			case 0: return x;
			case 1: return y;
		}

		throw "Vector index out of range!";
		return x;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator=(const Vector2<T>& value)
	{
		x = value.x;
		y = value.y;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator+(const Vector2<T>& value)
	{
		return Vector2<T>(*this) += value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator-(const Vector2<T>& value)
	{
		return Vector2<T>(*this) -= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator*(const Vector2<T>& value)
	{
		return Vector2<T>(*this) *= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator/(const Vector2<T>& value)
	{
		return Vector2<T>(*this) /= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator+(const T& value)
	{
		return Vector2<T>(*this) += value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator-(const T& value)
	{
		return Vector2<T>(*this) -= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator*(const T& value)
	{
		return Vector2<T>(*this) *= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T> Vector2<T>::operator/(const T& value)
	{
		return Vector2<T>(*this) /= value;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& value)
	{
		x += value.x;
		y += value.y;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& value)
	{
		x -= value.x;
		y -= value.y;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& value)
	{
		x *= value.x;
		y *= value.y;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator/=(const Vector2<T>& value)
	{
		x /= value.x;
		y /= value.y;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator+=(const T& value)
	{
		x += value;
		y += value;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator-=(const T& value)
	{
		x -= value;
		y -= value;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator*=(const T& value)
	{
		x *= value;
		y *= value;
		return *this;
	}

	template<typename T>
	TETHER_MATH_FUNC Vector2<T>& Vector2<T>::operator/=(const T& value)
	{
		x /= value;
		y /= value;
		return *this;
	}
}
