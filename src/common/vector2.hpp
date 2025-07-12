#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>
#include <limits>
#include <ostream>
#include <type_traits>
#include "../types.hpp"
#include "../util/fstream_reader.hpp"
#include "../util/fstream_writer.hpp"

template <typename T>
struct Vector2 {
	T x {};
	T y {};

	Vector2()          = default;
	virtual ~Vector2() = default;

	constexpr Vector2(T x_, T y_) noexcept
	    : x(x_)
	    , y(y_)
	{
	}

	Vector2(const Vector2&)            = default;
	Vector2(Vector2&&)                 = default;
	Vector2& operator=(const Vector2&) = default;
	Vector2& operator=(Vector2&&)      = default;

	[[nodiscard]] bool operator==(const Vector2& other) const noexcept
	{
		if constexpr (std::is_floating_point_v<T>) {
			return (std::abs(x - other.x) < std::numeric_limits<T>::epsilon())
			    && (std::abs(y - other.y) < std::numeric_limits<T>::epsilon());
		} else {
			return x == other.x && y == other.y;
		}
	}

	[[nodiscard]] bool operator!=(const Vector2& other) const noexcept { return !(*this == other); }

	virtual void read(util::fstream_reader&)  = 0;
	virtual void write(util::fstream_writer&) = 0;
};

struct Vector2f final : public Vector2<f32> {
	using Base = Vector2<f32>;

	Vector2f()           = default;
	~Vector2f() override = default;

	constexpr Vector2f(f32 x_, f32 y_) noexcept
	    : Base(x_, y_)
	{
	}

	Vector2f(const Vector2f&)            = default;
	Vector2f(Vector2f&&)                 = default;
	Vector2f& operator=(const Vector2f&) = default;
	Vector2f& operator=(Vector2f&&)      = default;

	void read(util::fstream_reader& reader) override;
	void write(util::fstream_writer& writer) override;

	friend std::ostream& operator<<(std::ostream& os, const Vector2f& v) { return os << v.x << ' ' << v.y; }
};

struct Vector2i final : public Vector2<u32> {
	using Base = Vector2<u32>;

	Vector2i()           = default;
	~Vector2i() override = default;

	constexpr Vector2i(u32 x_, u32 y_) noexcept
	    : Base(x_, y_)
	{
	}

	Vector2i(const Vector2i&)            = default;
	Vector2i(Vector2i&&)                 = default;
	Vector2i& operator=(const Vector2i&) = default;
	Vector2i& operator=(Vector2i&&)      = default;

	void read(util::fstream_reader& reader) override;
	void write(util::fstream_writer& writer) override;

	friend std::ostream& operator<<(std::ostream& os, const Vector2i& v) { return os << v.x << ' ' << v.y; }
};

#endif