#ifndef _TOKENISER_READER_HPP
#define _TOKENISER_READER_HPP

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <sstream>
#include <vector>
#include "../types.hpp"

namespace util {

class tokeniser {
public:
	tokeniser() = default;
	explicit tokeniser(const std::string& fileContents) { read(fileContents); }

	~tokeniser() = default;

	// Delete copy operations to prevent accidental copies
	tokeniser(const tokeniser&)            = delete;
	tokeniser& operator=(const tokeniser&) = delete;

	// Enable move operations for efficiency
	tokeniser(tokeniser&&)            = default;
	tokeniser& operator=(tokeniser&&) = default;

	void read(const std::string& fileContents)
	{
		m_tokenIdx = 0;
		m_tokenList.clear();
		m_tokenList.reserve((fileContents.size() / 5) + 1); // Rough estimate to reduce reallocations

		std::istringstream iss(fileContents);
		std::string token;

		while (iss >> token) {
			// Handle tokens that are in 'apostrophes' or "quotes"
			if (auto q1 = iterateUntilMatched(iss, token, '\'')) {
				token = std::move(*q1);
			} else if (auto q2 = iterateUntilMatched(iss, token, '"')) {
				token = std::move(*q2);
			}

			m_tokenList.push_back(token);
		}

		m_tokenList.shrink_to_fit();
	}

	[[nodiscard]] bool atWhiteSpace() const
	{
		if (isEnd()) {
			return false;
		}

		const auto& currentToken = getCurrentToken();
		return currentToken == " " || currentToken == "\t";
	}

	void reset() noexcept { m_tokenIdx = 0; }
	[[nodiscard]] std::size_t remainingTokens() const noexcept { return isEnd() ? 0 : m_tokenList.size() - m_tokenIdx; }

	[[nodiscard]] bool hasNext() const noexcept { return m_tokenIdx + 1 < m_tokenList.size(); }
	const std::string& next() { return m_tokenList[m_tokenIdx++]; }

	std::size_t seek(s32 amount) { return m_tokenIdx += amount; }
	[[nodiscard]] bool isEnd() const noexcept { return m_tokenIdx >= m_tokenList.size(); }

	[[nodiscard]] const std::vector<std::string>& getTokenList() const noexcept { return m_tokenList; }
	[[nodiscard]] std::size_t getTokenIndex() const noexcept { return m_tokenIdx; }
	[[nodiscard]] const std::string& getCurrentToken() const { return m_tokenList[m_tokenIdx]; }

private:
	std::vector<std::string> m_tokenList;
	std::size_t m_tokenIdx = 0;

	static std::optional<std::string> iterateUntilMatched(std::istringstream& iss, const std::string& currentToken, char identifier)
	{
		// Quick check if the token contains the character
		const auto firstItem = currentToken.find(identifier);
		if (firstItem == std::string::npos) {
			return std::nullopt;
		}

		// Check if we already have a complete string
		const auto secondItem = currentToken.find(identifier, firstItem + 1);
		if (secondItem != std::string::npos) {
			// Complete current token
			std::string result = currentToken;
			result.erase(std::remove(result.begin(), result.end(), identifier), result.end());
			return result;
		}

		// Need to read more tokens to find the closing quote
		std::string result = currentToken;
		std::string nextToken;

		while (iss >> nextToken) {
			result += ' ' + nextToken;

			if (nextToken.find(identifier) != std::string::npos) {
				// Found closing character, remove both quotes and return
				result.erase(std::remove(result.begin(), result.end(), identifier), result.end());
				return result;
			}
		}

		// Reached end of stream without finding closing character
		return std::nullopt;
	}
};

} // namespace util

#endif