#include <common/envelope.hpp>

void Envelope::read(util::fstream_reader& reader)
{
    m_indices.resize(reader.readU16());
    m_weights.reserve(m_indices.size());

    for (std::size_t i = 0; i < m_indices.size(); i++) {
        m_indices[i] = reader.readU16();
        m_weights[i] = reader.readF32();
    }
}

void Envelope::write(util::fstream_writer& writer)
{
    writer.writeU16(static_cast<u16>(m_indices.size()));

    for (std::size_t i = 0; i < m_indices.size(); i++) {
        writer.writeU16(m_indices[i]);
        writer.writeF32(m_weights[i]);
    }
}