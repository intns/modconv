#ifndef DMD_HPP
#define DMD_HPP

#include <common.hpp>
#include <types.hpp>
#include <util/tokeniser.hpp>

struct DMDHeader {
};

struct DMD {
    DMD()  = default;
    ~DMD() = default;

    void read(util::tokeniser& reader);
    void write(std::ofstream& os);
    void clear();
};

#endif