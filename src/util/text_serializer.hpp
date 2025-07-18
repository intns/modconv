#ifndef JSON_SERIALIZER_HPP
#define JSON_SERIALIZER_HPP

#pragma once

#include "serialization_base.hpp"
#include "../types.hpp"
#include <fstream>
#include <stack>
#include <algorithm>
#include <cctype>
#include <utility>
#include <stdexcept>

namespace serialization {
class JsonTextSerializer : public ISerializer {
private:
	std::ostream& m_out;
	int m_indentLevel = 0;
	std::stack<bool> m_needsComma; // Tracks if a comma is needed before the next element.

	void indent()
	{
		for (int i = 0; i < m_indentLevel; ++i)
			m_out << "    ";
	}

	void writeElementSeparator()
	{
		if (m_needsComma.empty())
			return;
		if (m_needsComma.top()) {
			m_out << ",\n";
		} else {
			m_out << "\n";
			m_needsComma.top() = true;
		}
	}

	template <typename T>
	void writeKeyValue(const std::string& key, const T& value)
	{
		writeElementSeparator();
		indent();
		m_out << "\"" << escapeString(key) << "\": " << value;
	}

	// Helper function to escape JSON strings
	std::string escapeString(const std::string& str)
	{
		std::string escaped;
		for (char c : str) {
			switch (c) {
			case '"':
				escaped += "\\\"";
				break;
			case '\\':
				escaped += "\\\\";
				break;
			case '\b':
				escaped += "\\b";
				break;
			case '\f':
				escaped += "\\f";
				break;
			case '\n':
				escaped += "\\n";
				break;
			case '\r':
				escaped += "\\r";
				break;
			case '\t':
				escaped += "\\t";
				break;
			default:
				if (c >= 0 && c < 32) {
					escaped += "\\u" + std::to_string(static_cast<int>(c));
				} else {
					escaped += c;
				}
				break;
			}
		}
		return escaped;
	}

public:
	explicit JsonTextSerializer(std::ostream& out)
	    : m_out(out)
	{
		m_out << std::setprecision(std::numeric_limits<double>::max_digits10);
	}

	void beginDocument() override
	{
		m_out << "{\n";
		m_indentLevel++;
		m_needsComma.push(false);
	}
	void endDocument() override
	{
		m_out << "\n}\n";
		m_out.flush();
	}

	void beginObject(const std::string& name = "") override
	{
		writeElementSeparator();
		indent();
		if (!name.empty()) {
			m_out << "\"" << escapeString(name) << "\": {";
		} else {
			m_out << "{";
		}
		m_indentLevel++;
		m_needsComma.push(false);
	}

	void endObject() override
	{
		m_indentLevel--;
		m_out << "\n";
		indent();
		m_out << "}";
		m_needsComma.pop();
	}

	void beginArray(const std::string& name = "") override
	{
		writeElementSeparator();
		indent();
		m_out << "\"" << escapeString(name) << "\": [";
		m_indentLevel++;
		m_needsComma.push(false);
	}

	void endArray() override
	{
		m_indentLevel--;
		m_out << "\n";
		indent();
		m_out << "]";
		m_needsComma.pop();
	}

	void write(const std::string& key, bool value) override { writeKeyValue(key, value ? "true" : "false"); }
	void write(const std::string& key, int value) override { writeKeyValue(key, value); }
	void writeU32(const std::string& key, u32 value) override { writeKeyValue(key, static_cast<uint64_t>(value)); }
	void write(const std::string& key, int64_t value) override { writeKeyValue(key, value); }
	void write(const std::string& key, float value) override
	{
		writeElementSeparator();
		indent();
		m_out << "\"" << escapeString(key) << "\": ";

		if (value == 0.0f && std::signbit(value)) {
			m_out << "-0.0"; // Explicit string for negative zero
		} else if (value == 0.0f) {
			m_out << "0.0"; // Always show decimal for zero
		} else {
			m_out << value;
		}
	}
	void write(const std::string& key, double value) override
	{
		writeElementSeparator();
		indent();
		m_out << "\"" << escapeString(key) << "\": ";

		if (value == 0.0f && std::signbit(value)) {
			m_out << "-0.0"; // Explicit string for negative zero
		} else if (value == 0.0f) {
			m_out << "0.0"; // Always show decimal for zero
		} else {
			m_out << value;
		}
	}
	void write(const std::string& key, const std::string& value) override
	{
		writeElementSeparator();
		indent();
		m_out << "\"" << escapeString(key) << "\": \"" << escapeString(value) << "\"";
	}

	void writeValue(bool value) override
	{
		writeElementSeparator();
		indent();
		m_out << (value ? "true" : "false");
	}
	void writeValue(int value) override
	{
		writeElementSeparator();
		indent();
		m_out << value;
	}
	void writeValue(int64_t value) override
	{
		writeElementSeparator();
		indent();
		m_out << value;
	}
	void writeValue(float value) override
	{
		writeElementSeparator();
		indent();
		m_out << value;
	}
	void writeValue(double value) override
	{
		writeElementSeparator();
		indent();
		m_out << value;
	}
	void writeValue(const std::string& value) override
	{
		writeElementSeparator();
		indent();
		m_out << "\"" << escapeString(value) << "\"";
	}

	void writeComment(const std::string& comment) override
	{
		// Write JSON5-style single-line comment
		writeElementSeparator();
		indent();
		m_out << "// " << comment;
		// Reset comma state since comments don't count as elements
		if (!m_needsComma.empty()) {
			m_needsComma.top() = false;
		}
	}
};

// Enhanced JSON deserializer with better error handling and parsing
class JsonTextDeserializer : public IDeserializer {
private:
	enum class TokenType { End, ObjectOpen, ObjectClose, ArrayOpen, ArrayClose, Comma, Colon, String, Number, Boolean, Null };

	struct Token {
		TokenType type;
		std::string value;
		size_t line   = 1;
		size_t column = 1;
	};

	std::vector<Token> m_tokens;
	size_t m_token_idx = 0;
	SerializationNode m_root;
	std::stack<SerializationNode*> m_nodeStack;
	SerializationNode* m_currentNode = nullptr;
	std::stack<size_t> m_arrayIndexStack;
	size_t m_currentArrayIndex = 0;

	// Enhanced tokenization with better error handling and comment support
	void tokenize(std::istream& in)
	{
		m_tokens.clear();
		char c;
		size_t line = 1, column = 1;

		while (in.get(c)) {
			if (c == '\n') {
				line++;
				column = 1;
				continue;
			}

			if (isspace(c)) {
				column++;
				continue;
			}

			// Handle comments
			if (c == '/') {
				char next = in.peek();
				if (next == '/') {
					// Single-line comment - skip to end of line
					in.get(); // consume second '/'
					column += 2;
					while (in.peek() != '\n' && in.peek() != EOF) {
						in.get();
						column++;
					}
					continue;
				} else if (next == '*') {
					// Multi-line comment - skip to */
					in.get(); // consume '*'
					column += 2;
					bool foundEnd = false;
					while (!foundEnd && in.peek() != EOF) {
						char ch = in.get();
						column++;
						if (ch == '\n') {
							line++;
							column = 1;
						} else if (ch == '*' && in.peek() == '/') {
							in.get(); // consume '/'
							column++;
							foundEnd = true;
						}
					}
					if (!foundEnd) {
						throw std::runtime_error("Unterminated multi-line comment starting at line " + std::to_string(line));
					}
					continue;
				}
				// If not a comment, fall through to handle as potential division operator
				// (though division isn't valid in JSON, we'll let the parser handle the error)
			}

			switch (c) {
			case '{':
				m_tokens.push_back({ TokenType::ObjectOpen, "{", line, column });
				break;
			case '}':
				m_tokens.push_back({ TokenType::ObjectClose, "}", line, column });
				break;
			case '[':
				m_tokens.push_back({ TokenType::ArrayOpen, "[", line, column });
				break;
			case ']':
				m_tokens.push_back({ TokenType::ArrayClose, "]", line, column });
				break;
			case ',':
				m_tokens.push_back({ TokenType::Comma, ",", line, column });
				break;
			case ':':
				m_tokens.push_back({ TokenType::Colon, ":", line, column });
				break;
			case '"': {
				std::string s;
				char next_char;
				size_t start_column = column;
				column++; // Skip opening quote

				while (in.get(next_char)) {
					column++;
					if (next_char == '"') {
						break;
					} else if (next_char == '\\') {
						if (in.get(next_char)) {
							column++;
							switch (next_char) {
							case '"':
								s += '"';
								break;
							case '\\':
								s += '\\';
								break;
							case '/':
								s += '/';
								break;
							case 'b':
								s += '\b';
								break;
							case 'f':
								s += '\f';
								break;
							case 'n':
								s += '\n';
								break;
							case 'r':
								s += '\r';
								break;
							case 't':
								s += '\t';
								break;
							case 'u': {
								// Unicode escape sequence \uXXXX
								std::string unicode_hex;
								for (int i = 0; i < 4; i++) {
									if (!in.get(next_char) || !isxdigit(next_char)) {
										throw std::runtime_error("Invalid unicode escape sequence at line " + std::to_string(line)
										                         + ", column " + std::to_string(column));
									}
									unicode_hex += next_char;
									column++;
								}
								// For simplicity, just add the raw unicode for now
								s += "\\u" + unicode_hex;
								break;
							}
							default:
								throw std::runtime_error("Invalid escape sequence at line " + std::to_string(line) + ", column "
								                         + std::to_string(column));
							}
						} else {
							throw std::runtime_error("Unexpected end of input in string at line " + std::to_string(line));
						}
					} else if (next_char == '\n') {
						throw std::runtime_error("Unterminated string at line " + std::to_string(line));
					} else {
						s += next_char;
					}
				}
				m_tokens.push_back({ TokenType::String, s, line, start_column });
				break;
			}
			default:
				if (isdigit(c) || c == '-') {
					std::string n;
					n += c;
					size_t start_column_num = column;

					// Handle negative sign
					if (c == '-') {
						if (!isdigit(in.peek())) {
							throw std::runtime_error("Invalid number format at line " + std::to_string(line) + ", column "
							                         + std::to_string(column));
						}
					}

					// Read digits and decimal point
					while (in.peek() != EOF
					       && (isdigit(in.peek()) || in.peek() == '.' || in.peek() == 'e' || in.peek() == 'E' || in.peek() == '+'
					           || in.peek() == '-')) {
						char peek_char = in.peek();
						// Handle scientific notation
						if (peek_char == 'e' || peek_char == 'E') {
							n += in.get();
							column++;
							if (in.peek() == '+' || in.peek() == '-') {
								n += in.get();
								column++;
							}
						} else {
							n += in.get();
							column++;
						}
					}

					// Validate number format
					try {
						if (n.find('.') != std::string::npos || n.find('e') != std::string::npos || n.find('E') != std::string::npos) {
							std::stod(n); // Test if it's a valid double
						} else {
							std::stoll(n); // Test if it's a valid long long
						}
					} catch (...) {
						throw std::runtime_error("Invalid number format '" + n + "' at line " + std::to_string(line) + ", column "
						                         + std::to_string(start_column_num));
					}

					m_tokens.push_back({ TokenType::Number, n, line, start_column_num });
				} else if (isalpha(c)) {
					std::string literal;
					literal += c;
					size_t start_column_lit = column;

					while (in.peek() != EOF && isalpha(in.peek())) {
						literal += in.get();
						column++;
					}

					// Determine token type based on literal value
					if (literal == "true" || literal == "false") {
						m_tokens.push_back({ TokenType::Boolean, literal, line, start_column_lit });
					} else if (literal == "null") {
						m_tokens.push_back({ TokenType::Null, literal, line, start_column_lit });
					} else {
						throw std::runtime_error("Invalid literal '" + literal + "' at line " + std::to_string(line) + ", column "
						                         + std::to_string(start_column_lit));
					}
				} else {
					throw std::runtime_error("Unexpected character '" + std::string(1, c) + "' at line " + std::to_string(line)
					                         + ", column " + std::to_string(column));
				}
				break;
			}
			column++;
		}
	}

	SerializationNode parseValue()
	{
		if (m_token_idx >= m_tokens.size()) {
			throw std::runtime_error("Unexpected end of input");
		}

		Token t = m_tokens[m_token_idx];

		switch (t.type) {
		case TokenType::ObjectOpen: {
			m_token_idx++; // Consume '{'
			SerializationNode node;
			node.makeObject();

			// Handle empty object: {}
			if (m_token_idx < m_tokens.size() && m_tokens[m_token_idx].type == TokenType::ObjectClose) {
				m_token_idx++;
				return node;
			}

			bool expectKey = true;
			while (m_token_idx < m_tokens.size()) {
				if (expectKey) {
					// Parse the key
					SerializationNode keyNode = parseValue();
					if (!keyNode.isString()) {
						throw std::runtime_error("Expected string key in object at line " + std::to_string(t.line));
					}

					// Consume the colon
					if (m_token_idx >= m_tokens.size() || m_tokens[m_token_idx].type != TokenType::Colon) {
						throw std::runtime_error("Expected ':' after object key at line " + std::to_string(t.line));
					}
					m_token_idx++;

					// Parse the value
					node.setMember(keyNode.getString(), parseValue());
					expectKey = false;
				}

				// After a key-value pair, we must see either '}' or ','
				if (m_token_idx >= m_tokens.size()) {
					throw std::runtime_error("Unexpected end of input in object");
				}

				if (m_tokens[m_token_idx].type == TokenType::ObjectClose) {
					m_token_idx++;
					break;
				}

				if (m_tokens[m_token_idx].type == TokenType::Comma) {
					m_token_idx++;
					expectKey = true;
				} else {
					throw std::runtime_error("Expected ',' or '}' in object at line " + std::to_string(m_tokens[m_token_idx].line));
				}
			}
			return node;
		}
		case TokenType::ArrayOpen: {
			m_token_idx++; // Consume '['
			SerializationNode node;
			node.makeArray();

			// Handle empty array: []
			if (m_token_idx < m_tokens.size() && m_tokens[m_token_idx].type == TokenType::ArrayClose) {
				m_token_idx++;
				return node;
			}

			bool expectValue = true;
			while (m_token_idx < m_tokens.size()) {
				if (expectValue) {
					node.pushBack(parseValue());
					expectValue = false;
				}

				// After a value, we must see either ']' or ','
				if (m_token_idx >= m_tokens.size()) {
					throw std::runtime_error("Unexpected end of input in array");
				}

				if (m_tokens[m_token_idx].type == TokenType::ArrayClose) {
					m_token_idx++;
					break;
				}

				if (m_tokens[m_token_idx].type == TokenType::Comma) {
					m_token_idx++;
					expectValue = true;
				} else {
					throw std::runtime_error("Expected ',' or ']' in array at line " + std::to_string(m_tokens[m_token_idx].line));
				}
			}
			return node;
		}
		default:
			m_token_idx++;
			if (t.type == TokenType::String)
				return SerializationNode(t.value);
			if (t.type == TokenType::Number) {
				try {
					if (t.value.find('.') != std::string::npos || t.value.find('e') != std::string::npos
					    || t.value.find('E') != std::string::npos) {
						return SerializationNode(std::stod(t.value));
					}

					auto s = std::stoll(t.value);
					if (s > std::numeric_limits<int32_t>::max() || s < std::numeric_limits<int32_t>::min()) {
						return SerializationNode(static_cast<int64_t>(s));
					}
					
					return SerializationNode(static_cast<int64_t>(std::stoll(t.value)));
				} catch (...) {
					throw std::runtime_error("Invalid number format at line " + std::to_string(t.line));
				}
			}
			if (t.type == TokenType::Boolean)
				return SerializationNode(t.value == "true");
			if (t.type == TokenType::Null)
				return SerializationNode(); // Return null node

			throw std::runtime_error("Unexpected token at line " + std::to_string(t.line));
		}
	}

public:
	explicit JsonTextDeserializer(std::istream& in)
	{
		try {
			tokenize(in);
			m_token_idx = 0;
			if (!m_tokens.empty()) {
				m_root = parseValue();
			}
			m_currentNode = &m_root;
		} catch (const std::exception& e) {
			throw std::runtime_error("JSON parsing failed: " + std::string(e.what()));
		}
	}

	bool parseDocument() override { return true; }

	bool enterObject(const std::string& name = "") override
	{
		SerializationNode* targetNode;
		if (name.empty()) {
			// Entering anonymous object in array
			if (!m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
				return false;
			targetNode = &(*m_currentNode)[m_currentArrayIndex];
		} else {
			// Entering named object
			if (!m_currentNode->isObject() || !m_currentNode->hasMember(name))
				return false;
			targetNode = &m_currentNode->getMember(name);
		}
		if (!targetNode || !targetNode->isObject())
			return false;
		m_nodeStack.push(m_currentNode);
		m_currentNode = targetNode;
		return true;
	}

	bool exitObject() override
	{
		if (m_nodeStack.empty())
			return false;
		m_currentNode = m_nodeStack.top();
		m_nodeStack.pop();
		return true;
	}

	bool enterArray(const std::string& name = "") override
	{
		if (!m_currentNode->isObject() || !m_currentNode->hasMember(name))
			return false;
		SerializationNode* targetNode = &m_currentNode->getMember(name);
		if (!targetNode || !targetNode->isArray())
			return false;
		m_nodeStack.push(m_currentNode);
		m_currentNode = targetNode;
		m_arrayIndexStack.push(m_currentArrayIndex);
		m_currentArrayIndex = 0;
		return true;
	}

	bool exitArray() override
	{
		if (m_nodeStack.empty())
			return false;
		m_currentNode = m_nodeStack.top();
		m_nodeStack.pop();
		if (!m_arrayIndexStack.empty()) {
			m_currentArrayIndex = m_arrayIndexStack.top();
			m_arrayIndexStack.pop();
		}
		return true;
	}

	size_t getArraySize() override { return m_currentNode->isArray() ? m_currentNode->arraySize() : 0; }

	// Fixed nextArrayElement method
	bool nextArrayElement() override
	{
		if (m_currentNode->isArray() && m_currentArrayIndex + 1 < m_currentNode->arraySize()) {
			m_currentArrayIndex++;
			return true;
		}
		return false;
	}

	bool hasKey(const std::string& key) override { return m_currentNode->isObject() && m_currentNode->hasMember(key); }

	// Enhanced read methods with better type conversion
	bool read(const std::string& key, bool& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (node.isBool()) {
			value = node.getBool();
			return true;
		}
		return false;
	}

	bool read(const std::string& key, int& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (node.isInt()) {
			value = static_cast<int>(node.getInt());
			return true;
		} else if (node.isFloat()) {
			// Allow conversion from float to int
			value = static_cast<int>(node.getFloat());
			return true;
		}
		return false;
	}

	bool readU32(const std::string& key, u32& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (node.isInt()) {
			int64_t intVal = node.getInt();
			if (intVal >= 0 && intVal <= UINT32_MAX) {
				value = static_cast<u32>(intVal);
				return true;
			}
		} else if (node.isFloat()) {
			double floatVal = node.getFloat();
			if (floatVal >= 0 && floatVal <= UINT32_MAX) {
				value = static_cast<u32>(floatVal);
				return true;
			}
		}
		return false;
	}

	bool read(const std::string& key, int64_t& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (node.isInt()) {
			value = node.getInt();
			return true;
		} else if (node.isFloat()) {
			value = static_cast<int64_t>(node.getFloat());
			return true;
		}
		return false;
	}

	bool read(const std::string& key, float& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (node.isFloat()) {
			value = static_cast<float>(node.getFloat());
			return true;
		} else if (node.isInt()) {
			value = static_cast<float>(node.getInt());
			return true;
		}
		return false;
	}

	bool read(const std::string& key, double& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (node.isFloat()) {
			value = node.getFloat();
			return true;
		} else if (node.isInt()) {
			value = static_cast<double>(node.getInt());
			return true;
		}
		return false;
	}

	bool read(const std::string& key, std::string& value) override
	{
		if (!hasKey(key))
			return false;
		const auto& node = m_currentNode->getMember(key);
		if (!node.isString())
			return false;
		value = node.getString();
		return true;
	}

	// Array value readers with better type handling
	bool readArrayValue(bool& value) override
	{
		if (!m_currentNode || !m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
			return false;
		const auto& node = (*m_currentNode)[m_currentArrayIndex];
		if (!node.isBool())
			return false;
		value = node.getBool();
		return true;
	}

	bool readArrayValue(int& value) override
	{
		if (!m_currentNode || !m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
			return false;
		const auto& node = (*m_currentNode)[m_currentArrayIndex];
		if (node.isInt()) {
			value = static_cast<int>(node.getInt());
			return true;
		} else if (node.isFloat()) {
			value = static_cast<int>(node.getFloat());
			return true;
		}
		return false;
	}

	bool readArrayValue(int64_t& value) override
	{
		if (!m_currentNode || !m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
			return false;
		const auto& node = (*m_currentNode)[m_currentArrayIndex];
		if (node.isInt()) {
			value = node.getInt();
			return true;
		} else if (node.isFloat()) {
			value = static_cast<int64_t>(node.getFloat());
			return true;
		}
		return false;
	}

	bool readArrayValue(float& value) override
	{
		if (!m_currentNode || !m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
			return false;
		const auto& node = (*m_currentNode)[m_currentArrayIndex];
		if (node.isFloat()) {
			value = static_cast<float>(node.getFloat());
			return true;
		} else if (node.isInt()) {
			value = static_cast<float>(node.getInt());
			return true;
		}
		return false;
	}

	bool readArrayValue(double& value) override
	{
		if (!m_currentNode || !m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
			return false;
		const auto& node = (*m_currentNode)[m_currentArrayIndex];
		if (node.isFloat()) {
			value = node.getFloat();
			return true;
		} else if (node.isInt()) {
			value = static_cast<double>(node.getInt());
			return true;
		}
		return false;
	}

	bool readArrayValue(std::string& value) override
	{
		if (!m_currentNode || !m_currentNode->isArray() || m_currentArrayIndex >= m_currentNode->arraySize())
			return false;
		const auto& node = (*m_currentNode)[m_currentArrayIndex];
		if (!node.isString())
			return false;
		value = node.getString();
		return true;
	}
};
} // namespace serialization

#endif // JSON_SERIALIZER_HPP