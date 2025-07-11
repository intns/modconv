#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

template <typename T>
struct Vector3Base {
	T x = 0, y = 0, z = 0;

	Vector3Base()  = default;
	virtual ~Vector3Base() = default;
	Vector3Base(T aX, T aY, T aZ)
	    : x(aX)
	    , y(aY)
	    , z(aZ)
	{
	}

	bool operator==(const Vector3Base<T>& other) const
	{
		if constexpr (std::is_floating_point_v<T>) {
			return (std::abs(x - other.x) < std::numeric_limits<T>::epsilon())
			    && (std::abs(y - other.y) < std::numeric_limits<T>::epsilon())
			    && (std::abs(z - other.z) < std::numeric_limits<T>::epsilon());
		} else {
			return x == other.x && y == other.y && z == other.z;
		}
	}

	virtual void read(util::fstream_reader&)  = 0;
	virtual void write(util::fstream_writer&) = 0;
};

struct Vector3f : public Vector3Base<f32> {
	Vector3f()  = default;
	virtual ~Vector3f() = default;
	Vector3f(f32 aX, f32 aY, f32 aZ)
	    : Vector3Base(aX, aY, aZ)
	{
	}

	void read(util::fstream_reader&) override;
	void write(util::fstream_writer&) override;
	friend std::ostream& operator<<(std::ostream& os, Vector3f v)
	{
		os << v.x << " " << v.y << " " << v.z;
		return os;
	}
};

struct Vector3i : public Vector3Base<u32> {
	Vector3i()  = default;
	virtual ~Vector3i() = default;
	Vector3i(u32 aX, u32 aY, u32 aZ)
	    : Vector3Base(aX, aY, aZ)
	{
	}

	void read(util::fstream_reader&) override;
	void write(util::fstream_writer&) override;
	friend std::ostream& operator<<(std::ostream& os, Vector3i v)
	{
		os << v.x << " " << v.y << " " << v.z;
		return os;
	}
};

#endif