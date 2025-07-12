#include <common/envelope.hpp>

void Envelope::read(util::fstream_reader& reader)
{
	mIndices.resize(reader.readU16());
	mWeights.resize(mIndices.size());

	for (std::size_t i = 0; i < mIndices.size(); i++) {
		mIndices[i] = reader.readU16();
		mWeights[i] = reader.readF32();
	}
}

void Envelope::write(util::fstream_writer& writer)
{
	writer.writeU16(static_cast<u16>(mIndices.size()));

	for (std::size_t i = 0; i < mIndices.size(); i++) {
		writer.writeU16(mIndices[i]);
		writer.writeF32(mWeights[i]);
	}
}