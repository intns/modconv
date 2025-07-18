#ifndef COMMON_DISPLAYLISTREADER_HPP
#define COMMON_DISPLAYLISTREADER_HPP

#include "../util/vector_reader.hpp"
#include "../types.hpp"
#include "mesh.hpp"
#include <span>
#include <optional>

// Primitive types from GX
enum class PrimitiveType : u8 { TriangleStrip = 0x98, Triangles = 0xA0 };

struct VertexAttrib {
	u8 mMatrixIndex   = 0;
	u8 mTexMtxIndex   = 0;
	u16 mPosition     = 0;
	u16 mNormal       = 0;
	u16 mColor        = 0;
	u16 mTexcoords[8] = {};

	// Helper to check if any texcoord is set
	bool hasTexcoords() const
	{
		for (auto tc : mTexcoords)
			if (tc != 0)
				return true;
		return false;
	}
};

struct Triangle {
	VertexAttrib mVertices[3];

	// Convenience accessors
	const VertexAttrib& operator[](size_t i) const { return mVertices[i]; }
	VertexAttrib& operator[](size_t i) { return mVertices[i]; }
};

struct FaceBatch {
	PrimitiveType mPrimType;
	std::vector<VertexAttrib> mVertices; // Raw vertex data
	std::vector<Triangle> mTriangles;    // Converted triangles

	// Convert strip/fan to triangles
	void convertToTriangles();

	const std::vector<Triangle>& getTriangles() const { return mTriangles; }
};

class DisplayListReader {
public:
	struct Options {
		bool mConvertToTriangles = true;  // Auto-convert strips to triangles
		bool mValidateData       = true;  // Validate indices
		size_t mMaxVertices      = 65536; // Max vertex count for validation

		Options() = default;
	};

	DisplayListReader(util::vector_reader& reader, u32 vcd);

	// Parse all batches
	std::vector<FaceBatch> parse();

	// Parse single batch (for streaming)
	std::optional<FaceBatch> parseNext();

	// Get statistics
	struct Stats {
		size_t mTotalVertices  = 0;
		size_t mTotalTriangles = 0;
		size_t mBatchCount     = 0;
		size_t mStripCount     = 0;
	};
	const Stats& getStats() const { return mStats; }

private:
	util::vector_reader& mReader;
	u32 mVCD;
	Options mOptions;
	Stats mStats;

	VertexAttrib readVertex();
	FaceBatch readBatch();
};

// Utility functions
namespace DListUtils {

// Optimize display list by merging compatible batches
std::vector<FaceBatch> optimizeBatches(const std::vector<FaceBatch>& batches);

// Convert to indexed triangle list
struct IndexedMesh {
	std::vector<VertexAttrib> vertices;
	std::vector<u16> indices;
};

IndexedMesh convertToIndexed(const std::vector<FaceBatch>& batches);
} // namespace DListUtils

#endif