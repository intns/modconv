#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

template <typename T> struct Vector2Base {
    T x = 0, y = 0;

    Vector2Base()  = default;
    ~Vector2Base() = default;
    Vector2Base(T aX, T aY)
        : x(aX)
        , y(aY)
    {
    }

    virtual void read(util::fstream_reader&)  = 0;
    virtual void write(util::fstream_writer&) = 0;
};

struct Vector2f : public Vector2Base<f32> {
    Vector2f()  = default;
    ~Vector2f() = default;
    Vector2f(f32 aX, f32 aY)
        : Vector2Base(aX, aY)
    {
    }

    void read(util::fstream_reader&) override;
    void write(util::fstream_writer&) override;
    friend std::ostream& operator<<(std::ostream& os, Vector2f v)
    {
        os << v.x << " " << v.y;
        return os;
    }
};

struct Vector2i : public Vector2Base<u32> {
    Vector2i()  = default;
    ~Vector2i() = default;
    Vector2i(u32 aX, u32 aY)
        : Vector2Base(aX, aY)
    {
    }

    void read(util::fstream_reader&) override;
    void write(util::fstream_writer&) override;
    friend std::ostream& operator<<(std::ostream& os, Vector2i v)
    {
        os << v.x << " " << v.y;
        return os;
    }
};

#endif