#include "dlist_reader.hpp"
#include <algorithm>
#include <unordered_map>

DisplayListReader::DisplayListReader(util::vector_reader& reader, u32 vcd)
    : mReader(reader)
    , mVCD(vcd)
{
}

VertexAttrib DisplayListReader::readVertex()
{
	VertexAttrib attr {};

	// Matrix indices (based on original code logic)
	if (mVCD & VCD::MatrixIndex) {
		attr.mMatrixIndex = mReader.readU8();
	}
	if (mVCD & VCD::TexMatrixIndex) {
		attr.mTexMtxIndex = mReader.readU8();
	}

	// Always present
	attr.mPosition = mReader.readU16();
	attr.mNormal   = mReader.readU16();

	// Color (optional)
	if (mVCD & VCD::Color0) {
		attr.mColor = mReader.readU16();
	}

	// Texture coordinates
	bool hasAnyTexcoord = false;
	for (int i = 0; i < 8; ++i) {
		if (mVCD & (VCD::Tex0 << i)) {
			attr.mTexcoords[i] = mReader.readU16();
			hasAnyTexcoord     = true;
		}
	}

	// CRITICAL: Based on the original code, if NO texcoords are present,
	// it still writes 2 bytes (0,0). We need to read them!
	if (!hasAnyTexcoord) {
		mReader.readU8();
		mReader.readU8();
	}

	return attr;
}

FaceBatch DisplayListReader::readBatch()
{
	FaceBatch batch;

	u8 primType = mReader.readU8();
	if (primType != 0x98 && primType != 0xA0) {
		// Push back the byte and return empty
		mReader.setPosition(mReader.getPosition() - 1);
		return batch;
	}

	batch.mPrimType = static_cast<PrimitiveType>(primType);
	u16 vertexCount = mReader.readU16();

	// Reserve space
	batch.mVertices.reserve(vertexCount);

	// Read all vertices
	for (u16 i = 0; i < vertexCount; ++i) {
		batch.mVertices.push_back(readVertex());
	}

	// Update stats
	mStats.mTotalVertices += vertexCount;
	if (batch.mPrimType == PrimitiveType::TriangleStrip) {
		mStats.mStripCount++;
	}

	// Auto-convert to triangles if requested
	if (mOptions.mConvertToTriangles) {
		batch.convertToTriangles();
		mStats.mTotalTriangles += batch.mTriangles.size();
	}

	return batch;
}

std::optional<FaceBatch> DisplayListReader::parseNext()
{
	if (mReader.getRemaining() == 0) {
		return std::nullopt;
	}

	FaceBatch batch = readBatch();
	if (batch.mVertices.empty()) {
		return std::nullopt;
	}

	mStats.mBatchCount++;
	return batch;
}

std::vector<FaceBatch> DisplayListReader::parse()
{
	std::vector<FaceBatch> batches;

	while (auto batch = parseNext()) {
		batches.push_back(std::move(*batch));
	}

	return batches;
}

void FaceBatch::convertToTriangles()
{
	if (mVertices.empty())
		return;

	mTriangles.clear();

	if (mPrimType == PrimitiveType::Triangles) {
		// Direct triangles - every 3 vertices form a triangle
		size_t triCount = mVertices.size() / 3;
		mTriangles.reserve(triCount);

		for (size_t i = 0; i < triCount; ++i) {
			Triangle tri;
			tri.mVertices[0] = mVertices[i * 3];
			tri.mVertices[1] = mVertices[i * 3 + 1];
			tri.mVertices[2] = mVertices[i * 3 + 2];
			mTriangles.push_back(tri);
		}
	} else if (mPrimType == PrimitiveType::TriangleStrip) {
		// Triangle strip - sliding window
		if (mVertices.size() < 3)
			return;

		size_t triCount = mVertices.size() - 2;
		mTriangles.reserve(triCount);

		for (size_t i = 0; i < triCount; ++i) {
			Triangle tri;
			if (i & 1) {
				// Odd triangles are reversed for correct winding
				tri.mVertices[0] = mVertices[i + 1];
				tri.mVertices[1] = mVertices[i];
				tri.mVertices[2] = mVertices[i + 2];
			} else {
				// Even triangles
				tri.mVertices[0] = mVertices[i];
				tri.mVertices[1] = mVertices[i + 1];
				tri.mVertices[2] = mVertices[i + 2];
			}
			mTriangles.push_back(tri);
		}
	}
}

// Utility implementations
namespace DListUtils {

IndexedMesh convertToIndexed(const std::vector<FaceBatch>& batches)
{
	IndexedMesh mesh;

	// Hash function for vertex deduplication
	struct VertexHash {
		size_t operator()(const VertexAttrib& v) const
		{
			size_t h = 0;
			h ^= std::hash<u16> {}(v.mPosition) << 1;
			h ^= std::hash<u16> {}(v.mNormal) << 2;
			h ^= std::hash<u16> {}(v.mColor) << 3;
			h ^= std::hash<u8> {}(v.mMatrixIndex) << 4;
			h ^= std::hash<u8> {}(v.mTexMtxIndex) << 5;
			for (int i = 0; i < 8; ++i) {
				h ^= std::hash<u16> {}(v.mTexcoords[i]) << (6 + i);
			}
			return h;
		}
	};

	struct VertexEqual {
		bool operator()(const VertexAttrib& a, const VertexAttrib& b) const
		{
			return a.mPosition == b.mPosition && a.mNormal == b.mNormal && a.mColor == b.mColor && a.mMatrixIndex == b.mMatrixIndex
			    && a.mTexMtxIndex == b.mTexMtxIndex
			    && std::equal(std::begin(a.mTexcoords), std::end(a.mTexcoords), std::begin(b.mTexcoords));
		}
	};

	std::unordered_map<VertexAttrib, u16, VertexHash, VertexEqual> vertexMap;

	for (const auto& batch : batches) {
		// Ensure triangles are generated
		const_cast<FaceBatch&>(batch).convertToTriangles();

		for (const auto& tri : batch.mTriangles) {
			for (int i = 0; i < 3; ++i) {
				const auto& v = tri.mVertices[i];

				auto it = vertexMap.find(v);
				if (it == vertexMap.end()) {
					u16 idx      = static_cast<u16>(mesh.vertices.size());
					vertexMap[v] = idx;
					mesh.vertices.push_back(v);
					mesh.indices.push_back(idx);
				} else {
					mesh.indices.push_back(it->second);
				}
			}
		}
	}

	return mesh;
}

std::vector<FaceBatch> optimizeBatches(const std::vector<FaceBatch>& batches)
{
	// Simple optimization: merge consecutive batches with same primitive type
	std::vector<FaceBatch> optimized;

	for (const auto& batch : batches) {
		if (!optimized.empty() && optimized.back().mPrimType == batch.mPrimType
		    && optimized.back().mVertices.size() + batch.mVertices.size() < 65536) {
			// Merge
			auto& last = optimized.back();
			last.mVertices.insert(last.mVertices.end(), batch.mVertices.begin(), batch.mVertices.end());
			last.mTriangles.clear(); // Will be regenerated
		} else {
			optimized.push_back(batch);
		}
	}

	return optimized;
}

} // namespace DListUtils