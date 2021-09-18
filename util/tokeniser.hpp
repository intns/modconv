#ifndef _TOKENISER_READER_HPP
#define _TOKENISER_READER_HPP

#include <optional>
#include <sstream>
#include <string>
#include <types.hpp>
#include <vector>

namespace util {

class tokeniser {
public:
    tokeniser() = default;
    inline tokeniser(const std::string& fileContents)
    {
        read(fileContents);
    }

    ~tokeniser() = default;

    inline void read(const std::string& fileContents)
    {
        m_tokenIdx = 0;
        m_tokenList.clear();

        std::istringstream iss(fileContents);
        std::string token = "";
        while (iss >> token) {
            // Handle tokens that are in 'apostrophes' or "quotes"
            auto iterApostrophe = iterateUntilMatched(iss, token, '\'');
            if (iterApostrophe.has_value()) {
                token = iterApostrophe.value();
            }

            auto iterQuotes = iterateUntilMatched(iss, token, '\"');
            if (iterQuotes.has_value()) {
                token = iterQuotes.value();
            }

            m_tokenList.push_back(token);
        }
    }

    inline const bool atWhiteSpace() const
    {
        const std::string currentToken = getCurrentToken();
        return currentToken.length() == 1 && currentToken == " " || currentToken == "\t";
    }

    inline const std::string& next() { return m_tokenList[m_tokenIdx++]; }
    inline const std::size_t seek(s32 amount) { return m_tokenIdx += amount; }
    inline const bool isEnd() const { return m_tokenIdx == m_tokenList.size(); }

    inline const std::vector<std::string> getTokenList() const { return m_tokenList; }
    inline const std::size_t getTokenIndex() const { return m_tokenIdx; }
    inline const std::string getCurrentToken() const { return m_tokenList[m_tokenIdx]; }

private:
    std::vector<std::string> m_tokenList;
    std::size_t m_tokenIdx = 0;

    inline std::optional<std::string> iterateUntilMatched(std::istringstream& iss, const std::string& lookingIn, char identifier)
    {
        if (std::count(lookingIn.begin(), lookingIn.end(), identifier)) {
            std::string line = lookingIn;
            std::string lookingInCopy = "";
            u32 identifiersPresent = 0;

            while (identifiersPresent != 2) {
                iss >> lookingInCopy;
                line += lookingInCopy;

                identifiersPresent = std::count(line.begin(), line.end(), identifier);
            }

            auto found = std::find(line.begin(), line.end(), identifier);
            line.erase(found);

            found = std::find(line.begin(), line.end(), identifier);
            line.erase(found);

            return line;
        }

        return std::nullopt;
    }
};

}

#endif