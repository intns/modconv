#ifndef _FSTREAM_READER_HPP
#define _FSTREAM_READER_HPP

#include <fstream>
#include <type_traits>
#include <types.hpp>
#include <vector>

namespace util {

class fstream_reader : public std::ifstream {
public:
    enum class Endianness : u8 { Little = 0, Big };

    ~fstream_reader() = default;
    fstream_reader(std::size_t position = 0, Endianness endianness = Endianness::Little);
    fstream_reader(const fstream_reader&) = delete;
    fstream_reader& operator=(const fstream_reader&) = delete;

    inline const std::streampos getRemaining() { return m_filesize - tellg(); }
    inline const std::streampos getFilesize() const { return m_filesize; }

    inline Endianness& endianness() { return m_endianness; }
    inline const Endianness& endianness() const { return m_endianness; }

    inline void open_fstream(std::size_t position = 0, Endianness endianness = Endianness::Little)
    {
        m_endianness = endianness;
        seekg(0, std::ios_base::end);
        m_filesize = tellg();
        setPosition(position);
    }

    inline void setPosition(std::size_t position)
    {
        if (position <= m_filesize) {
            seekg(position);
        }
    }

    inline void read_buffer(char* buffer, std::streampos size) { read(buffer, size); }

    inline u8 readU8()
    {
        u8 byte = 0;
        read(reinterpret_cast<char*>(&byte), 1);
        return byte;
    }
    inline u16 readU16()
    {
        u8 byte0 = readU8();
        u8 byte1 = readU8();

        return (m_endianness == Endianness::Little) ? (byte0 | (byte1 << 8)) : (byte1 | (byte0 << 8));
    }
    inline u32 readU32()
    {
        u32 v = 0;
        read(reinterpret_cast<char*>(&v), 4);

        return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8)
                | ((v & 0x000000ff) << 24));
    }
    inline u64 readU64()
    {
        u32 word0 = readU32();
        u32 word1 = readU32();

        return (m_endianness == Endianness::Little) ? (((u64)word0) | ((u64)word1 << 32))
                                                    : (((u64)word1) | ((u64)word0 << 32));
    }

    inline s8 readS8()
    {
        char byte = 0;
        read(&byte, 1);
        return byte;
    }
    inline s16 readS16()
    {
        s8 byte0 = readS8();
        s8 byte1 = readS8();

        return (m_endianness == Endianness::Little) ? (byte0 | (byte1 << 8)) : (byte1 | (byte0 << 8));
    }
    inline s32 readS32()
    {
        s8 byte0 = readS8();
        s8 byte1 = readS8();
        s8 byte2 = readS8();
        s8 byte3 = readS8();

        return (m_endianness == Endianness::Little) ? (byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24))
                                                    : (byte3 | (byte2 << 8) | (byte1 << 16) | (byte0 << 24));
    }
    inline s64 readS64()
    {
        s32 word0 = readS32();
        s32 word1 = readS32();

        return (m_endianness == Endianness::Little) ? (((s64)word0 << 32) | (word1)) : (((s64)word1 << 32) | (word0));
    }

    inline f32 readF32()
    {
        u32 word = readU32();
        return *reinterpret_cast<float*>(&word);
    }

private:
    std::streampos m_filesize;
    Endianness m_endianness;
};

} // namespace util

#endif