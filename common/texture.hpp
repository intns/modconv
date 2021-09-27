#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct Texture {
    u16 m_width   = 0;
    u16 m_height  = 0;
    u32 m_format  = 0;
    u32 m_unknown = 0;
    std::vector<u8> m_imageData;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

struct TextureAttributes {
    u16 m_index      = 0;
    u16 m_tilingMode = 0;
    u16 m_unknown1   = 0;
    f32 m_unknown2   = 0;

    void read(util::fstream_reader&);
    void write(util::fstream_writer&);
};

#endif