#ifndef SERIALIZATION_UTILS_HPP
#define SERIALIZATION_UTILS_HPP

#pragma once

#include "serialization_base.hpp"
#include "text_serializer.hpp"
#include <fstream>
#include <memory>

namespace serialization {

// Helper templates for serializing common types
template <typename T>
void serializeVector(ISerializer& s, const std::string& name, const std::vector<T>& vec)
{
	s.beginArray(name);
	for (const auto& item : vec) {
		if constexpr (std::is_base_of_v<ISerializable, T>) {
			s.beginObject("");
			item.serialize(s);
			s.endObject();
		} else {
			s.writeValue(item);
		}
	}
	s.endArray();
}

template <typename T>
bool deserializeVector(IDeserializer& d, const std::string& name, std::vector<T>& vec)
{
	vec.clear();
	if (!d.enterArray(name))
		return false;

	size_t size = d.getArraySize();
	vec.reserve(size);

	for (size_t i = 0; i < size; ++i) {
		T item;
		if constexpr (std::is_base_of_v<ISerializable, T>) {
			if (d.enterObject("")) {
				item.deserialize(d);
				d.exitObject();
				vec.push_back(std::move(item));
			}
		} else {
			if (d.readArrayValue(item)) {
				vec.push_back(item);
			}
		}

		// Only advance if not at last element
		if (i < size - 1) {
			d.nextArrayElement();
		}
	}

	d.exitArray();
	return true;
}

// Serialize basic math types (you'll need to adapt these to your math library)
template <typename Vec2>
void serializeVec2(ISerializer& s, const std::string& name, const Vec2& v)
{
	s.beginObject(name);
	s.write("x", v.x);
	s.write("y", v.y);
	s.endObject();
}

template <typename Vec2>
bool deserializeVec2(IDeserializer& d, const std::string& name, Vec2& v)
{
	if (!d.enterObject(name))
		return false;
	d.read("x", v.x);
	d.read("y", v.y);
	d.exitObject();
	return true;
}

template <typename Vec3>
void serializeVec3(ISerializer& s, const std::string& name, const Vec3& v)
{
	s.beginObject(name);
	s.write("x", v.x);
	s.write("y", v.y);
	s.write("z", v.z);
	s.endObject();
}

template <typename Vec3>
bool deserializeVec3(IDeserializer& d, const std::string& name, Vec3& v)
{
	if (!d.enterObject(name))
		return false;
	d.read("x", v.x);
	d.read("y", v.y);
	d.read("z", v.z);
	d.exitObject();
	return true;
}

template <typename Color>
void serializeColor(ISerializer& s, const std::string& name, const Color& c)
{
	s.beginObject(name);
	s.write("r", c.r);
	s.write("g", c.g);
	s.write("b", c.b);
	s.write("a", c.a);
	s.endObject();
}

template <typename Color>
bool deserializeColor(IDeserializer& d, const std::string& name, Color& c)
{
	if (!d.enterObject(name))
		return false;
	int r, g, b, a;
	d.read("r", r);
	d.read("g", g);
	d.read("b", b);
	d.read("a", a);
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	d.exitObject();
	return true;
}

// Enum serialization helpers
template <typename E>
void serializeEnum(ISerializer& s, const std::string& name, E value, const std::function<std::string(E)>& toString)
{
	s.write(name, toString(value));
}

template <typename E>
bool deserializeEnum(IDeserializer& d, const std::string& name, E& value, const std::function<E(const std::string&)>& fromString)
{
	std::string str;
	if (d.read(name, str)) {
		value = fromString(str);
		return true;
	}
	return false;
}

// Flags serialization (for bitfields)
template <typename F>
void serializeFlags(ISerializer& s, const std::string& name, F flags, const std::vector<std::pair<F, std::string>>& flagNames)
{
	s.beginArray(name);
	for (const auto& [flag, flagName] : flagNames) {
		if (flags & flag) {
			s.writeValue(flagName);
		}
	}
	s.endArray();
}

template <typename F>
bool deserializeFlags(IDeserializer& d, const std::string& name, F& flags, const std::unordered_map<std::string, F>& flagMap)
{
	flags = static_cast<F>(0);
	if (!d.enterArray(name))
		return false;

	size_t size = d.getArraySize();
	for (size_t i = 0; i < size; ++i) {
		std::string flagName;
		if (d.readArrayValue(flagName)) {
			auto it = flagMap.find(flagName);
			if (it != flagMap.end()) {
				flags = static_cast<F>(flags | it->second);
			}
		}
		if (i < size - 1)
			d.nextArrayElement();
	}

	d.exitArray();
	return true;
}

} // namespace serialization

#endif // SERIALIZATION_UTILS_HPP