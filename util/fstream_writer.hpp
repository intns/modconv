#ifndef _FSTREAM_WRITER_HPP
#define _FSTREAM_WRITER_HPP

#include <fstream>
#include <type_traits>
#include <types.hpp>
#include <vector>

namespace util {

class fstream_writer : public std::ofstream {
public:
    enum class Endianness : u8 {
        Little = 0,
        Big
    };

    fstream_writer(Endianness endianness);
    ~fstream_writer() = default;
    fstream_writer(const fstream_writer&) = delete;
    fstream_writer& operator=(const fstream_writer&) = delete;

    inline Endianness& endianness() { return m_endianness; }
    inline const Endianness& endianness() const { return m_endianness; }

    inline void align(u32 amt)
    {
        const u32 pos = static_cast<u32>(tellp());
        for (u32 i = 0; i < ((~(amt - 1) & (pos + amt - 1)) - pos); ++i) {
            writeU8(0);
        }
    }

    inline void writeU8(u8 val)
    {
        write(reinterpret_cast<char*>(&val), 1);
    }
    inline void writeU16(u16 val)
    {
        u16 value = ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
        write(reinterpret_cast<char*>(&value), 2);
    }
    inline void writeU32(u32 val)
    {
        u32 value = ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
        write(reinterpret_cast<char*>(&value), 4);
    }

    inline void writeS8(s8 val)
    {
        write(reinterpret_cast<char*>(&val), 1);
    }
    inline void writeS16(s16 val)
    {
        s16 value = ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
        write(reinterpret_cast<char*>(&value), 2);
    }
    inline void writeS32(s32 val)
    {
        s32 value = ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
        write(reinterpret_cast<char*>(&value), 4);
    }

    inline void writeF32(f32 val)
    {
        u32 intVal = reinterpret_cast<u32&>(val);
        u32 value = ((intVal & 0xff000000) >> 24) | ((intVal & 0x00ff0000) >> 8) | ((intVal & 0x0000ff00) << 8) | ((intVal & 0x000000ff) << 24);
        write(reinterpret_cast<char*>(&value), 4);
    }

private:
    Endianness m_endianness;
};

}

#endif