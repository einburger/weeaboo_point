#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <functional>

typedef unsigned char uchar;

namespace detail {

    template<typename U, typename V, std::size_t... I, typename Op>
    auto impl_op(const U& lhs, const V& rhs, std::index_sequence<I...>, Op op) 
    {  
        if constexpr (std::is_scalar_v<V>)
            return std::array<typename U::value_type, sizeof...(I)>{ { op(lhs[I],    rhs) ... } };
        else if constexpr (std::is_scalar_v<U>)
            return std::array<typename V::value_type, sizeof...(I)>{ { op(lhs,    rhs[I]) ... } };
        else  // then both are array types
            return std::array<typename U::value_type, sizeof...(I)>{ { op(lhs[I], rhs[I]) ... } };
    } 

    template<typename U, typename V, std::size_t N, std::size_t... I, typename Op>
    void impl_op_eq(U& lhs, V& rhs, std::index_sequence<I...>, Op op) 
    {  
        if constexpr (std::is_scalar_v<V>)
            ( (lhs[I] = op(lhs[I],  rhs)), ... ); 
        else if constexpr (std::is_scalar_v<U>)
            ( (rhs[I] = op(lhs,     rhs[I])), ... ); 
        else // then both are array types
            ( (lhs[I] = op(lhs[I],  rhs[I])), ... ); 
    }
    
}  // end namespace detail


// operator overloads for generic arrays of different sizes
template<typename T, std::size_t N, typename U, std::size_t M, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightArithmetic=std::is_arithmetic_v<U> >
auto operator+(const std::array<T, N>& lhs, const std::array<U, M>& rhs) {
    static_assert(isLeftArithmetic && isRightArithmetic, 
                  "One operand has no arithmetic types.");
    return detail::impl_op(lhs, rhs, std::make_index_sequence<std::min(N,M)>{}, std::plus<>{});
}

template<typename T, std::size_t N, typename U, std::size_t M, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightArithmetic=std::is_arithmetic_v<U> >
auto operator-(const std::array<T, N>& lhs, const std::array<U, M>& rhs) {
    static_assert(isLeftArithmetic && isRightArithmetic, 
                  "One operand has no arithmetic types.");
    return detail::impl_op(lhs, rhs, std::make_index_sequence<std::min(N,M)>{}, std::minus<>{});
}

template<typename T, std::size_t N, typename U, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightScalar=std::is_scalar_v<U> >
auto operator*(std::array<T, N>& lhs, const U& scale) {
    static_assert(isLeftArithmetic && isRightScalar, 
                  "Left operand has no arithmetic types or right isn't scalar.");
    detail::impl_op(lhs, scale, std::make_index_sequence<N>{}, std::multiplies<>{});
    return lhs;
}

template<typename T, std::size_t N, typename U, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightScalar=std::is_scalar_v<U> >
auto operator/(std::array<T, N>& lhs, const U& scale) {
    static_assert(isLeftArithmetic && isRightScalar, 
                  "Left operand has no arithmetic types or right isn't scalar.");
    detail::impl_op(lhs, scale, std::make_index_sequence<N>{}, std::divides<>{});
    return lhs;
}

template<typename T, std::size_t N, typename U, std::size_t M, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightArithmetic=std::is_arithmetic_v<U> >
auto& operator+=(std::array<T, N>& lhs, const std::array<U, M>& rhs) {
    static_assert(isLeftArithmetic && isRightArithmetic, 
                  "One operand has no arithmetic types.");
    detail::impl_op_eq(lhs, rhs, std::make_index_sequence<std::min(N,M)>{}, std::plus<>{});
    return lhs;
}

template<typename T, std::size_t N, typename U, std::size_t M, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightArithmetic=std::is_arithmetic_v<U> >
auto& operator-=(std::array<T, N>& lhs, const std::array<U, M>& rhs) {
    static_assert(isLeftArithmetic && isRightArithmetic, 
                  "One operand has no arithmetic types.");
    detail::impl_op_eq(lhs, rhs, std::make_index_sequence<std::min(N,M)>{}, std::minus<>{});
    return lhs;
}

template<typename T, std::size_t N, typename U, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightScalar=std::is_scalar_v<U> >
auto& operator*=(std::array<T, N>& lhs, const U& scale) {
    static_assert(isLeftArithmetic && isRightScalar, 
                  "Left operand has no arithmetic types or right isn't scalar.");
    detail::impl_op_eq(lhs, scale, std::make_index_sequence<N>{}, std::multiplies<>{});
    return lhs;
}

template<typename T, std::size_t N, typename U, 
         bool isLeftArithmetic =std::is_arithmetic_v<T>, 
         bool isRightScalar=std::is_scalar_v<U> >
auto& operator/=(std::array<T, N>& lhs, const U& scale) {
    static_assert(isLeftArithmetic && isRightScalar, 
                  "Left operand has no arithmetic types or right isn't scalar.");
    detail::impl_op_eq(lhs, scale, std::make_index_sequence<N>{}, std::divides<>{});
    return lhs;
}



struct Sprite
{
	unsigned int texture{};
	std::array<int, 2> w_h{};
};


struct Box
{
	std::string name{};

	bool clicked{false};
	std::array<int, 2> clicked_x_y{};

	std::array<int, 2> min_xy{};
	std::array<int, 2> max_xy{};
	std::array<int, 2> w_h{};
        std::array<float, 4> rgba{1.0f, 1.0f, 1.0f, 1.0f};

	/* should be animation object */
	float wait_time{0.0f};
	float speed{5.0f};
	float fade_speed{0.05f};
	int target_pos{};

	Sprite sprite;
	std::unordered_map<std::string, std::string> emotion_map; // should be in a different class

	Box() = default;

	Box(const int x, const int y, const int w, const int h) :
		min_xy({x, y}), max_xy({x+w, y+h}), w_h({w,h}) {}

	void set_position(const std::array<int, 2> &xy) {
		min_xy = xy;
                max_xy = min_xy + w_h;
	}

	void set_color(const std::array<float, 4> &color) { rgba = color; }

	void set_size(const std::array<int, 2>& width_height) {
                max_xy = min_xy + width_height;
		w_h = width_height;
	}

	void scale();

	void set_texture(const std::string &file_name);
	void set_emotion(const std::string &emotion);

	void draw();

	// animation related
	bool move(const int x, const int y, const int speed);
	bool wait(const double seconds);
	bool fade(const float speed);

        std::string print() {
            return  "alpha: " + std::to_string(rgba[3])  + " " + 
                    + "x_pos: " + std::to_string(min_xy[0]);
        }
};

using Character = Box;



void geometry_line_draw(int x0, int y0, int x1, int y1);

