#pragma once

#include <array>
#include <string>
#include <unordered_set>
#include <functional>

typedef unsigned char uchar;

// Array overlaods
namespace detail {

	template<typename U, typename V, std::size_t... I, typename Op>
	auto impl_op(const U& lhs, const V& rhs, std::index_sequence<I...>, Op op)
	{
		if constexpr (std::is_scalar_v<V>)
			return std::array<typename U::value_type, sizeof...(I)>{ { op(lhs[I], rhs) ... } };
		else if constexpr (std::is_scalar_v<U>)
			return std::array<typename V::value_type, sizeof...(I)>{ { op(lhs, rhs[I]) ... } };
		else  // then both are array types
			return std::array<typename U::value_type, sizeof...(I)>{ { op(lhs[I], rhs[I]) ... } };
	}

	template<typename U, typename V, std::size_t N, std::size_t... I, typename Op>
	void impl_op_eq(U& lhs, V& rhs, std::index_sequence<I...>, Op op)
	{
		if constexpr (std::is_scalar_v<V>)
			((lhs[I] = op(lhs[I], rhs)), ...);
		else if constexpr (std::is_scalar_v<U>)
			((rhs[I] = op(lhs, rhs[I])), ...);
		else // then both are array types
			((lhs[I] = op(lhs[I], rhs[I])), ...);
	}

}  // end namespace detail


// operator overloads for generic arrays of different sizes
template<typename T, std::size_t N, typename U, std::size_t M,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightArithmetic = std::is_arithmetic_v<U> >
	auto operator+(const std::array<T, N>& lhs, const std::array<U, M>& rhs)
{
	static_assert(isLeftArithmetic && isRightArithmetic,
				  "One operand has no arithmetic types.");
	return detail::impl_op(lhs, rhs, std::make_index_sequence<std::min(N, M)>{}, std::plus<>{});
}

template<typename T, std::size_t N, typename U, std::size_t M,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightArithmetic = std::is_arithmetic_v<U> >
	auto operator-(const std::array<T, N>& lhs, const std::array<U, M>& rhs)
{
	static_assert(isLeftArithmetic && isRightArithmetic,
				  "One operand has no arithmetic types.");
	return detail::impl_op(lhs, rhs, std::make_index_sequence<std::min(N, M)>{}, std::minus<>{});
}

template<typename T, std::size_t N, typename U,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightScalar = std::is_scalar_v<U> >
	auto operator*(std::array<T, N>& lhs, const U& scale)
{
	static_assert(isLeftArithmetic && isRightScalar,
				  "Left operand has no arithmetic types or right isn't scalar.");
	detail::impl_op(lhs, scale, std::make_index_sequence<N>{}, std::multiplies<>{});
	return lhs;
}

template<typename T, std::size_t N, typename U,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightScalar = std::is_scalar_v<U> >
	auto operator/(std::array<T, N>& lhs, const U& scale)
{
	static_assert(isLeftArithmetic && isRightScalar,
				  "Left operand has no arithmetic types or right isn't scalar.");
	detail::impl_op(lhs, scale, std::make_index_sequence<N>{}, std::divides<>{});
	return lhs;
}

template<typename T, std::size_t N, typename U, std::size_t M,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightArithmetic = std::is_arithmetic_v<U> >
	auto& operator+=(std::array<T, N>& lhs, const std::array<U, M>& rhs)
{
	static_assert(isLeftArithmetic && isRightArithmetic,
				  "One operand has no arithmetic types.");
	detail::impl_op_eq(lhs, rhs, std::make_index_sequence<std::min(N, M)>{}, std::plus<>{});
	return lhs;
}

template<typename T, std::size_t N, typename U, std::size_t M,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightArithmetic = std::is_arithmetic_v<U> >
	auto& operator-=(std::array<T, N>& lhs, const std::array<U, M>& rhs)
{
	static_assert(isLeftArithmetic && isRightArithmetic,
				  "One operand has no arithmetic types.");
	detail::impl_op_eq(lhs, rhs, std::make_index_sequence<std::min(N, M)>{}, std::minus<>{});
	return lhs;
}

template<typename T, std::size_t N, typename U,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightScalar = std::is_scalar_v<U> >
	auto& operator*=(std::array<T, N>& lhs, const U& scale)
{
	static_assert(isLeftArithmetic && isRightScalar,
				  "Left operand has no arithmetic types or right isn't scalar.");
	detail::impl_op_eq(lhs, scale, std::make_index_sequence<N>{}, std::multiplies<>{});
	return lhs;
}

template<typename T, std::size_t N, typename U,
	bool isLeftArithmetic = std::is_arithmetic_v<T>,
	bool isRightScalar = std::is_scalar_v<U> >
	auto& operator/=(std::array<T, N>& lhs, const U& scale)
{
	static_assert(isLeftArithmetic && isRightScalar,
				  "Left operand has no arithmetic types or right isn't scalar.");
	detail::impl_op_eq(lhs, scale, std::make_index_sequence<N>{}, std::divides<>{});
	return lhs;
}


struct Sprite
{
	unsigned int	   texture{};
	std::array<int, 2> w_h{};
	std::string		   path;
};

struct Textured 
{
	Sprite						sprite{}; // not needed
	std::vector<Sprite>			sprites{};
	std::vector<std::string>	sprite_paths{ {} };
	unsigned int				sprite_paths_idx{};

	void set_texture(const std::string& fullpath);
};

struct Physical : public Textured
{
	std::array<int, 2>		min_xy{0,0};
	std::array<int, 2>		max_xy{1,1};
	std::array<int, 2>		w_h{1,1};
	std::array<float, 4>	rgba{ 1.0f, 1.0f, 1.0f, 0.0f };

	inline void set_pos(int x, int y)
	{
		min_xy[0] = x;
		min_xy[1] = y;
		max_xy[0] = x + w_h[0];
		max_xy[1] = y + w_h[1];
	}

	inline void set_to_sprite_size()
	{
		set_size(sprite.w_h[0], sprite.w_h[1]);
	}

	inline void set_size(int width, int height)
	{
		w_h = std::array{ width, height };
		max_xy = min_xy + std::array{ width, height };
	}

	void scale_to_screen();

	inline void set_color(std::array<float, 4>& color)
	{
		rgba = color;
	}

	inline void set_alpha(float alpha)
	{
		rgba[3] = alpha;
	}

	void draw();
};

struct Animatable : public Physical
{
	float wait_time{ 0.0f };
	float speed{ 5.0f };
	float fade_speed{ 0.05f };
	int	  target_pos{0};

	bool move(const int x, const int y, const int speed);
	bool wait(const double seconds);
	bool fade(const float speed);
};

struct Character : public Animatable
{
	std::string name{};

	Character() = default;
	Character(const std::string& name) {
		this->name = name;
	}
	Character(const std::string& name, std::string& spritepath) {
		this->name = name;
		set_texture(spritepath);
		set_to_sprite_size();
	}
	Character(const int x_min, const int y_min, const int w, const int h) 
	{  
		min_xy  = std::array{ x_min, y_min };
		w_h		= std::array{ w, h };
		max_xy  = std::array{ x_min + w, y_min + h };
	}

	std::string to_string()
	{
		return name + " xpos " + std::to_string(min_xy[0]) 
				    + " alpha " + std::to_string(rgba[3]);
	}
};


// void geometry_line_draw(int x0, int y0, int x1, int y1);


