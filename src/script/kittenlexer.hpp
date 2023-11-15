#ifndef KITTEN_LEXER_HPP
#define KITTEN_LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <stack>
#include <map>

struct KittenToken {
    std::string src;  // The content of the token
    bool str = false;  // Indicates if the token is a string
    unsigned long line = 0;
};

using lexed_kittens = std::vector<KittenToken>;

class KittenLexer {
    using storage = std::map<char, bool>;
    storage stringqs;
    std::vector<std::tuple<char, char>> capsules;
    storage ignores;
    storage as_newline;
    storage extracts;
    storage lineskips;
    std::map<char, char> backslash_opts;
    bool erase_emptys = false;
    bool ignore_backslash_ops = false;
    bool failbit = false;  // Flag indicating lexer failure

    using conditional_func = bool(*)(char c);
    using conditional_storage = std::vector<conditional_func>;
    conditional_storage con_ignores;
    conditional_storage con_newlines;
    conditional_storage con_extracts;
    conditional_storage con_lineskips;
    conditional_storage con_stringqs;

    /**
     * Checks if the given character matches any of the conditions in the conditional storage.
     *
     * @param f the conditional storage containing the conditions to match against
     * @param c the character to be checked
     *
     * @return true if the character matches any of the conditions, false otherwise
     */
    inline bool match_con_any(const conditional_storage& f, char c) {
        for(auto i : f) if(i(c)) return true;
        return false;
    }

public:
    inline bool is_stringq(char c) const { 
        return stringqs.count(c) > 0;
    }
    /**
     * Checks if the given characters are part of a capsule.
     *
     * @param open the open character of the capsule
     * @param close the close character of the capsule
     *
     * @return true if the characters are part of a capsule, false otherwise
     */
    inline bool is_capsule(char open, char close) const {
        for(auto tu : capsules) {
            auto [o, c] = tu;
            if(o == open || o == close || c == open || c == close) return true;
        }
        return false;
    }
    inline bool is_capsule_open(char ch) const {
        for(auto tu : capsules) {
            auto [o, c] = tu;
            if(o == ch) return true;
        }
        return false;
    }
    inline bool is_capsule_close(char ch) const {
        for(auto tu : capsules) {
            auto [o, c] = tu;
            if(c == ch) return true;
        }
        return false;
    }
    inline bool match_closure(char open, char close) const {
        for(auto tu : capsules) {
            auto [o, c] = tu;
            if(o == open && c == close) return true;
        }
        return false;
    }
    inline bool is_ignore(char c) const {
        return ignores.count(c) > 0;
    }
    inline bool is_newline(char c) const {
        return as_newline.count(c) > 0;
    }
    inline bool is_extract(char c) const {
        return extracts.count(c) > 0;
    }
    inline bool is_lineskip(char c) const {
        return lineskips.count(c) > 0;
    }


    inline KittenLexer& add_capsule(char open, char close) {
        if(!is_capsule(open,close))
            capsules.push_back(std::make_tuple(open, close));
        return *this;
    }
    inline KittenLexer& add_stringq(char c) {
        if(!is_stringq(c))
            stringqs[c] = true;
        return *this;
    }
    inline KittenLexer& add_ignore(char c) {
        if(!is_ignore(c))
            ignores[c] = true;
        return *this;
    }
    inline KittenLexer& add_extract(char c) {
        if(!is_extract(c))
            extracts[c] = true;
        return *this;
    }
    inline KittenLexer& add_lineskip(char c) {
        if(!is_lineskip(c))
            lineskips[c] = true;
        return *this;
    }
    inline KittenLexer& erase_empty() {
        this->erase_emptys = true;
        return *this;
    }
    inline KittenLexer& ignore_backslash_opts() {
        this->ignore_backslash_ops = true;
        return *this;
    }
    inline KittenLexer& add_linebreak(char c) {
        if(!is_newline(c))
            as_newline[c] = true;
        return *this;
    }
    inline KittenLexer& add_backslashopt(char c, char to) {
        backslash_opts[c] = to;
        return *this;
    }

    inline KittenLexer& add_con_stringq(conditional_func func) {
        con_stringqs.push_back(func);
        return *this;
    }
    inline KittenLexer& add_con_extract(conditional_func func) {
        con_extracts.push_back(func);
        return *this;
    }
    inline KittenLexer& add_con_lineskip(conditional_func func) {
        con_lineskips.push_back(func);
        return *this;
    }
    inline KittenLexer& add_con_ignore(conditional_func func) {
        con_ignores.push_back(func);
        return *this;
    }
    inline KittenLexer& add_con_newline(conditional_func func) {
        con_newlines.push_back(func);
        return *this;
    }

    /**
     * Lexes a given string and returns a vector of tokens.
     *
     * @param src the string to be lexed
     *
     * @return a vector of lexed tokens
     *
     * @throws ErrorType if there is an error during lexing
     */
    inline lexed_kittens lex(std::string src) { //const &
        lexed_kittens ret;  // Resulting vector of KittenTokens
        std::stack<char> opens; // Stack to track opening capsule characters
        std::stack<char> stringqs;  // Stack to track string quote characters
        bool suntil_newline = false;  // Flag indicating line skipping
        unsigned long line = 1; // Line number in the source code
        KittenToken token;  // Current token being constructed

        // Iterate through each character in the source code
        for(size_t i = 0; i < src.size(); ++i) {
            // Check for newline character
            if(is_newline(src[i]) || match_con_any(con_newlines, src[i])) {
                ++line;
            }

            // Check for characters to be ignored
            if(stringqs.empty() && opens.empty() && (is_ignore(src[i]) || is_newline(src[i]) || match_con_any(con_newlines, src[i]) || match_con_any(con_ignores,src[i]))) {
                if(is_newline(src[i]) || match_con_any(con_newlines, src[i])) suntil_newline = false;
                if((is_ignore(src[i]) || match_con_any(con_ignores, src[i])) && suntil_newline) continue; 
                if(token.src != "" || !erase_emptys) {
                    token.line = line - 1 * (is_newline(src[i]) || match_con_any(con_newlines, src[i]));
                    ret.push_back(token);
                    token = {};
                }
            }
            // Check for opening capsule character
            else if(stringqs.empty() && is_capsule_open(src[i]) && !suntil_newline) {
                // If the stack of openings is empty and there's a non-empty token, add it to the result vector
                if(opens.empty() && (token.src != "" || !erase_emptys)) {
                    if(token.src == "") token.line = line;
                    ret.push_back(token);
                    token = {};
                    token.line = line;
                }
                // Track the opening capsule character
                opens.push(src[i]);
                if(token.src == "") token.line = line;
                token.src += src[i];
            }
            // Check for closing capsule character
            else if(stringqs.empty() && is_capsule_close(src[i]) && !suntil_newline) {
                // If the stack of openings is empty or the top of the stack doesn't match the current closing character,
                // set failbit and return
                if(opens.empty() || !match_closure(opens.top(), src[i])) {
                    failbit = true;
                    return ret;
                }

                // Add the closing capsule character to the current token
                token.src += src[i];
                opens.pop();

                // If all capsules are closed, add the token to the result vector
                if(opens.empty()) {
                    if(token.src == "") token.line = line;
                    ret.push_back(token);
                    token = {};
                }

                // Update line number for the next token
                if(token.src == "") token.line = line;
            }
            // Check for string quote character
            else if(opens.empty() && (is_stringq(src[i]) || match_con_any(con_stringqs, src[i])) && !suntil_newline) {
                // If the stack of string quotes is empty, start a new string
                if(stringqs.empty()) {
                    if(token.src != "" || !erase_emptys) {
                        if(token.src == "") token.line = line;
                        ret.push_back(token);
                        token = {};
                    }

                    // Track the string quote character
                    stringqs.push(src[i]);
                }
                else if(stringqs.top() == src[i]) {
                    // If the current string quote character matches the top of the stack, end the string
                    stringqs.pop();
                    token.str = true;
                    ret.push_back(token);
                    token = {};
                }
                else {
                    // Continue building the string
                    token.src += src[i];
                }
            }
            // Check for extraction character
            else if(opens.empty() && stringqs.empty() && (is_extract(src[i]) || match_con_any(con_extracts, src[i])) && !suntil_newline) {
                if(token.src != "" || !erase_emptys)
                    ret.push_back(KittenToken{token.src, false, line});

                // Add the extraction character as a separate token
                ret.push_back(KittenToken{std::string(1, src[i]), false, line});
                token = {};
            }
            // Check for line skipping character
            else if(stringqs.empty() && (is_lineskip(src[i]) || match_con_any(con_lineskips, src[i]))) {
                // Enable line skipping until the next newline character
                suntil_newline = true;
            }
            else if((is_newline(src[i]) || match_con_any(con_newlines, src[i])) && stringqs.empty() && suntil_newline) {
                suntil_newline = false;
            }
            // Check for backslash escape character
            else if(opens.empty() && /*stringqs.empty() &&*/ src[i] == '\\') {
                // If backslash escape characters are to be ignored, add it to the current token and continue
                if(ignore_backslash_ops) {
                    token.src += src[i];
                    continue;
                }

                // Process the backslash escape character
                if(i + 1 != src.size()) {
                    char n = src[i + 1];
                    if(backslash_opts.count(n) != 0) token.src += backslash_opts[n];
                    else token.src += n;
                    ++i;
                }
                else {
                    // Incomplete backslash escape, set failbit and return
                    failbit = true;
                    return ret;
                }
            }
            // Add character to the current token
            else if(!suntil_newline) {
                if(token.src == "") token.line = line;

                // Add the current character to the token
                token.src += src[i];
            }
        }

        // Check for unmatched capsules or string quotes
        if(!opens.empty() || !stringqs.empty()) {
            failbit = true;
            return ret;
        }

        if(token.src != "") {
            ret.push_back(token);
        }

        // Return the resulting vector of tokens
        return ret;
    }

    inline operator bool() {
        return !failbit;
    }
};

#endif