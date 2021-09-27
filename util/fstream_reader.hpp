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

    inline void align(u32 amt)
    {
        u32 offs = amt - (tellg() % amt);
        if (offs != amt) {
            seekg(offs, std::ios_base::cur);
        }
    }

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
        u16 v = 0;
        read(reinterpret_cast<char*>(&v), 2);
        return ((v & 0xff00) >> 8) | ((v & 0x00ff) << 8);
    }
    inline u32 readU32()
    {
        u32 v = 0;
        read(reinterpret_cast<char*>(&v), 4);
        return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8)
                | ((v & 0x000000ff) << 24));
    }

    inline s8 readS8()
    {
        char byte = 0;
        read(&byte, 1);
        return byte;
    }
    inline s16 readS16()
    {
        s16 v = 0;
        read(reinterpret_cast<char*>(&v), 2);
        return ((v & 0xff00) >> 8) | ((v & 0x00ff) << 8);
    }
    inline s32 readS32()
    {
        s32 v = 0;
        read(reinterpret_cast<char*>(&v), 4);
        return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8)
                | ((v & 0x000000ff) << 24));
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