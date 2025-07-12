#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <types.hpp>
#include <util/fstream_reader.hpp>
#include <util/fstream_writer.hpp>

struct Envelope {
	std::vector<s16> mIndices;
	std::vector<f32> mWeights;

	void read(util::fstream_reader&);
	void write(util::fstream_writer&);
};

#endif