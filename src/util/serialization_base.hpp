#ifndef SERIALIZATION_BASE_HPP
#define SERIALIZATION_BASE_HPP

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <typeinfo>
#include <sstream>
#include <iomanip>
#include <unordered_map>

namespace serialization {

// Forward declarations
class SerializationNode;
class ISerializer;
class IDeserializer;

// Type traits for serialization
template <typename T>
struct is_serializable_primitive {
	static constexpr bool value = std::is_arithmetic_v<T> || std::is_same_v<T, std::string>;
};

// Base interface for serializable objects
class ISerializable {
public:
	virtual ~ISerializable()                              = default;
	virtual void serialize(ISerializer& serializer) const = 0;
	virtual void deserialize(IDeserializer& deserializer) = 0;
};

// Node representing a value in the serialization tree
class SerializationNode {
public:
	enum class Type { Null, Bool, Int, Float, String, Array, Object };

private:
	Type m_type = Type::Null;
	std::string m_stringValue;
	std::vector<SerializationNode> m_arrayValue;
	std::unordered_map<std::string, SerializationNode> m_objectValue;

	union {
		bool m_boolValue;
		int64_t m_intValue;
		double m_floatValue;
	};

public:
	SerializationNode()
	    : m_type(Type::Null)
	{
	}

	// Constructors for different types
	SerializationNode(bool value)
	    : m_type(Type::Bool)
	    , m_boolValue(value)
	{
	}
	explicit SerializationNode(int value)
	    : m_type(Type::Int)
	    , m_intValue(static_cast<int64_t>(value))
	{
	}
	SerializationNode(int64_t value)
	    : m_type(Type::Int)
	    , m_intValue(value)
	{
	}
	explicit SerializationNode(float value)
	    : m_type(Type::Float)
	    , m_floatValue(value)
	{
	}
	SerializationNode(double value)
	    : m_type(Type::Float)
	    , m_floatValue(value)
	{
	}
	SerializationNode(const std::string& value)
	    : m_type(Type::String)
	    , m_stringValue(value)
	{
	}
	SerializationNode(const char* value)
	    : m_type(Type::String)
	    , m_stringValue(value)
	{
	}

	// Type checking
	Type getType() const { return m_type; }
	bool isNull() const { return m_type == Type::Null; }
	bool isBool() const { return m_type == Type::Bool; }
	bool isInt() const { return m_type == Type::Int; }
	bool isFloat() const { return m_type == Type::Float; }
	bool isString() const { return m_type == Type::String; }
	bool isArray() const { return m_type == Type::Array; }
	bool isObject() const { return m_type == Type::Object; }

	// Value getters
	bool getBool() const { return m_boolValue; }
	int64_t getInt() const { return m_intValue; }
	double getFloat() const { return m_floatValue; }
	const std::string& getString() const { return m_stringValue; }

	// Array operations
	void makeArray() { m_type = Type::Array; }
	void pushBack(const SerializationNode& node)
	{
		if (m_type != Type::Array)
			makeArray();
		m_arrayValue.push_back(node);
	}
	size_t arraySize() const { return m_arrayValue.size(); }
	const SerializationNode& operator[](size_t index) const { return m_arrayValue[index]; }
	SerializationNode& operator[](size_t index) { return m_arrayValue[index]; }

	// Object operations
	void makeObject() { m_type = Type::Object; }
	void setMember(const std::string& key, const SerializationNode& value)
	{
		if (m_type != Type::Object)
			makeObject();
		m_objectValue[key] = value;
	}
	bool hasMember(const std::string& key) const { return m_objectValue.find(key) != m_objectValue.end(); }
	const SerializationNode& getMember(const std::string& key) const
	{
		static SerializationNode nullNode;
		auto it = m_objectValue.find(key);
		return (it != m_objectValue.end()) ? it->second : nullNode;
	}
	SerializationNode& getMember(const std::string& key)
	{
		if (m_type != Type::Object)
			makeObject();
		return m_objectValue[key];
	}
	const std::unordered_map<std::string, SerializationNode>& getMembers() const { return m_objectValue; }
};

// Serializer interface
class ISerializer {
public:
	virtual ~ISerializer() = default;

	// Start/end document
	virtual void beginDocument() = 0;
	virtual void endDocument()   = 0;

	// Object/array handling
	virtual void beginObject(const std::string& name = "") = 0;
	virtual void endObject()                               = 0;
	virtual void beginArray(const std::string& name = "")  = 0;
	virtual void endArray()                                = 0;

	// Write primitives
	virtual void write(const std::string& key, bool value)               = 0;
	virtual void write(const std::string& key, int value)                = 0;
	virtual void writeU32(const std::string& key, u32 value)             = 0;
	virtual void write(const std::string& key, int64_t value)            = 0;
	virtual void write(const std::string& key, float value)              = 0;
	virtual void write(const std::string& key, double value)             = 0;
	virtual void write(const std::string& key, const std::string& value) = 0;

	// Write without key (for arrays)
	virtual void writeValue(bool value)               = 0;
	virtual void writeValue(int value)                = 0;
	virtual void writeValue(int64_t value)            = 0;
	virtual void writeValue(float value)              = 0;
	virtual void writeValue(double value)             = 0;
	virtual void writeValue(const std::string& value) = 0;

	// Comments
	virtual void writeComment(const std::string& comment) = 0;
};

// Deserializer interface
class IDeserializer {
public:
	virtual ~IDeserializer() = default;

	// Parse document
	virtual bool parseDocument() = 0;

	// Navigation
	virtual bool enterObject(const std::string& name = "") = 0;
	virtual bool exitObject()                              = 0;
	virtual bool enterArray(const std::string& name = "")  = 0;
	virtual bool exitArray()                               = 0;

	// Read primitives
	virtual bool read(const std::string& key, bool& value)        = 0;
	virtual bool read(const std::string& key, int& value)         = 0;
	virtual bool readU32(const std::string& key, u32& value)      = 0;
	virtual bool read(const std::string& key, int64_t& value)     = 0;
	virtual bool read(const std::string& key, float& value)       = 0;
	virtual bool read(const std::string& key, double& value)      = 0;
	virtual bool read(const std::string& key, std::string& value) = 0;

	// Read array values
	virtual bool readArrayValue(bool& value)        = 0;
	virtual bool readArrayValue(int& value)         = 0;
	virtual bool readArrayValue(int64_t& value)     = 0;
	virtual bool readArrayValue(float& value)       = 0;
	virtual bool readArrayValue(double& value)      = 0;
	virtual bool readArrayValue(std::string& value) = 0;

	// Array info
	virtual size_t getArraySize()   = 0;
	virtual bool nextArrayElement() = 0;

	// Check if key exists
	virtual bool hasKey(const std::string& key) = 0;
};

// Helper macros for easy serialization
#define SERIALIZE_FIELD(serializer, field)     serializer.write(#field, field)
#define DESERIALIZE_FIELD(deserializer, field) deserializer.read(#field, field)

#define SERIALIZE_OBJECT(serializer, name, object) \
	serializer.beginObject(name);                  \
	object.serialize(serializer);                  \
	serializer.endObject()

#define DESERIALIZE_OBJECT(deserializer, name, object) \
	if (deserializer.enterObject(name)) {              \
		object.deserialize(deserializer);              \
		deserializer.exitObject();                     \
	}

} // namespace serialization

#endif // SERIALIZATION_BASE_HPP