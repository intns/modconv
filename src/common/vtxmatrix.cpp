#include "vtxmatrix.hpp"

void VtxMatrix::read(util::fstream_reader& reader)
{
	int weights        = reader.readS16();
	mHasPartialWeights = weights >= 0;
	if (mHasPartialWeights) {
		mIndex = weights;
	} else {
		mIndex = -1 - weights;
	}
}

void VtxMatrix::write(util::fstream_writer& writer) const
{
	if (mHasPartialWeights) {
		writer.writeS16(static_cast<s16>(mIndex));
	} else {
		writer.writeS16(static_cast<s16>(-1 - mIndex));
	}
}
