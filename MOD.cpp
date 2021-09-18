#include "MOD.hpp"

void MOD::align(util::fstream_reader& reader, u32 amt)
{
    u32 offs = amt - (reader.m_filestream.tellg() % amt);
    if (offs != 0x20) {
        reader.m_filestream.seekg(offs, std::ios_base::cur);
    }
}

void MOD::read(util::fstream_reader& reader)
{
}