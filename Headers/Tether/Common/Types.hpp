#ifndef _TETHER_TYPES_HPP
#define _TETHER_TYPES_HPP

#include <iostream>

namespace Tether
{
	template<typename T>
	class Vector2
	{
	public:
		Vector2() {}
		Vector2(T value) 
			:
			x(value),
			y(value)
		{}
		Vector2(T x, T y) 
			:
			x(x),
			y(y)
		{}

		// The ref is the one to copy from (I always forget that)
		Vector2(const Vector2& ref)
		{
			x = ref.x;
			y = ref.y;
		}

		T x = 0.0f;
		T y = 0.0f;
	};

	template<typename T>
	class Vector3
	{
	public:
		Vector3() {}
		Vector3(T value) 
			:
			x(value),
			y(value),
			z(value)
		{}
		Vector3(T x, T y, T z) 
			:
			x(x),
			y(y),
			z(z)
		{}

		// The ref is the one to copy from (I always forget that)
		Vector3(const Vector3& ref)
		{
			x = ref.x;
			y = ref.y;
			z = ref.z;
		}

		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;
	};

	template<typename T>
	class Vector4
	{
	public:
		Vector4() {}
		Vector4(T value) 
			:
			x(value),
			y(value),
			z(value),
			w(value)
		{}
		Vector4(T x, T y, T z, T w) 
			:
			x(x),
			y(y),
			z(z),
			w(w)
		{}

		// The ref is the one to copy from (I always forget that)
		Vector4(const Vector4& ref)
		{
			x = ref.x;
			y = ref.y;
			z = ref.z;
			w = ref.w;
		}
		
		T x = 0.0f;
		T y = 0.0f;
		T z = 0.0f;
		T w = 0.0f;
	};

	typedef Vector2<float> Vector2f;
	typedef Vector3<float> Vector3f;
	typedef Vector4<float> Vector4f;

	enum class ErrorCode
	{
		FAILED_APP_INIT,
		DOUBLE_INIT,
		NOT_INITIALIZED,
	};

	enum class ErrorSeverity
	{
		NOTE = 0,
		LOW = 1,
		MEDIUM = 2,
		HIGH = 3,
	};
}

#endif //_TETHER_TYPES_HPP