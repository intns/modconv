#include <common/vtxmatrix.hpp>

void VtxMatrix::read(util::fstream_reader& reader) { mIndex = reader.readU16(); }

void VtxMatrix::write(util::fstream_writer& writer) const { writer.writeU16(mIndex); }
