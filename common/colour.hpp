#ifndef COLOUR_HPP
#define COLOUR_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

template <typename T> struct ColourBase {
    T r = 0, g = 0, b = 0, a = 0;

    virtual void read(util::fstream_reader&)  = 0;
    virtual void write(util::fstream_writer&) = 0;
};

struct ColourU8 : public ColourBase<u8> {
    void read(util::fstream_reader&) override;
    void write(util::fstream_writer&) override;
    friend std::ostream& operator<<(std::ostream&, ColourU8 const&);
};

struct ColourU16 : public ColourBase<u16> {
    void read(util::fstream_reader&) override;
    void write(util::fstream_writer&) override;
    friend std::ostream& operator<<(std::ostream&, ColourU16 const&);
};

#endif