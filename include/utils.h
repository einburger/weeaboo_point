#pragma once
#include <array>
#include <type_traits>

template<class T>
struct v2
{
	T x = 1;
	T y = 1;

	v2() = default;
	v2(const T& a, const T& b) {
		x = a;
		y = b;
	}

	v2<T>& operator=(const v2<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	v2<T>& operator+=(const v2<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	v2<T>& operator-=(const v2<T>& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	v2<T>& operator*=(const v2<T>& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}
	v2<T>& operator/=(const v2<T>& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}
};

template <class T>
struct v4
{
	std::array<T, 4> arr{0};
	T& r = arr[0];
	T& g = arr[1];
	T& b = arr[2];
	T& a = arr[3];

	v4() = default;
	v4(const T& a, const T& b, const T& c, const T& d) { 
		arr[0] = a;
		arr[1] = b;
		arr[2] = c;
		arr[3] = d;
	}
	v4(const v4<T> &other) : arr(other.arr) {
		r = arr[0];
		b = arr[1];
		g = arr[2];
		a = arr[3];
	}

	T& operator[](size_t i)
	{ return arr[i]; }

	v4<T>& operator=(const v4<T>& rhs)
	{
		arr[0] = rhs.arr[0];
		arr[1] = rhs.arr[1];
		arr[2] = rhs.arr[2];
		arr[3] = rhs.arr[3];
		r = arr[0];
		g = arr[1];
		b = arr[2];
		a = arr[3];
		return *this;
	}

	T* data()
	{
		return &arr[0];
	}

};

/* 2 element array overloads */
template<class T>
v2<T> operator+(const v2<T>& lhs, const v2<T>& rhs)
{
	return v2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}
template<class T>
v2<T> operator-(const v2<T>& lhs, const v2<T>& rhs)
{
	return v2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}
template<class T>
v2<T> operator*(const v2<T>& lhs, const v2<T>& rhs)
{
	return v2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}
template<class T, class U>
v2<T> operator*(const v2<T>& lhs, U rhs)
{
	static_assert(std::is_scalar<U>::value, "rhs wasn't scalar");
	return v2<T>(static_cast<T>(static_cast<U>(lhs.x) * rhs), 
				 static_cast<T>(static_cast<U>(lhs.y) * rhs));
}
template<class T>
v2<T> operator/(const v2<T>& lhs, const v2<T>& rhs)
{
	return v2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}
template<class T, class U>
v2<T> operator/(const v2<T>& lhs, U rhs)
{
	static_assert(std::is_scalar<U>::value, "rhs wasn't scalar");
	return v2<T>(static_cast<T>(static_cast<U>(lhs.x) / rhs), 
				 static_cast<T>(static_cast<U>(lhs.y) / rhs));
}

/* 4 element array overloads */
template<class T>
v4<T> operator+(const v4<T>& lhs, const v4<T>& rhs)
{
	return v4<T>{lhs.arr[0] + rhs.arr[0], lhs.arr[1] + rhs.arr[1], lhs.arr[2] + rhs.arr[2], lhs.arr[3] + rhs.arr[3]};
}
template<class T>
v4<T> operator-(const v4<T>& lhs, const v4<T>& rhs)
{
	return v4<T>{lhs.arr[0] - rhs.arr[0], lhs.arr[1] - rhs.arr[1], lhs.arr[2] - rhs.arr[2], lhs.arr[3] - rhs.arr[3]};
}
template<class T>
v4<T> operator*(const v4<T>& lhs, const v4<T>& rhs)
{
	return v4<T>{lhs.arr[0] * rhs.arr[0], lhs.arr[1] * rhs.arr[1], lhs.arr[2] * rhs.arr[2], lhs.arr[3] * rhs.arr[3]};
}
template<class T>
v4<T> operator/(const v4<T>& lhs, const v4<T>& rhs)
{
	return v4<T>{lhs.arr[0] / rhs.arr[0], lhs.arr[1] / rhs.arr[1], lhs.arr[2] / rhs.arr[2], lhs.arr[3] / rhs.arr[3]};
}
