#ifndef CARESCRIPT_PARSING_HPP
#define CARESCRIPT_PARSING_HPP

#include "carescript-defs.hpp"
#include "carescript-defaults.hpp"

#include <string.h>
#include <filesystem>
#include <variant>

// Implementation for the functions declared in "carescript-defs.hpp"

///
//TODO add get_ext() for windows (ugh)
//TODO way to define rules using tags
///

#ifdef _WIN32
# include <windows.h>
namespace carescript {
inline static ExtensionData get_ext(std::filesystem::path name) noexcept { return ExtensionData{}; }
#elif defined(__linux__)
# include <dlfcn.h>
namespace carescript {
inline static ExtensionData get_ext(std::filesystem::path name) noexcept {
    if(!name.has_extension()) name += ".so";
    if(name.is_relative())
        name = "./" + name.string();
    void* handler = dlopen(name.c_str(),RTLD_NOW);
    if(handler == nullptr) return {0,nullptr};
    get_extension_fun f = (get_extension_fun)dlsym(handler,"get_extension");
    if(f == nullptr) return {0,nullptr};
    return f();
}
#endif

inline static bool bake_extension(ExtensionData ext, ScriptSettings& settings) noexcept {
    if(ext.extension == nullptr) return false;

    ext.extension->interp_link = &settings.interpreter;
    BuiltinList b_list = ext.extension->get_builtins();
    settings.interpreter.script_builtins.insert(b_list.begin(),b_list.end());
    TypeList t_list = ext.extension->get_types();
    for(auto i : t_list) settings.interpreter.script_typechecks.push_back(i);
    OperatorList o_list = ext.extension->get_operators();
    for(auto i : o_list) {
        for(auto j : i.second) {
            settings.interpreter.script_operators[i.first].push_back(j);
        }
    }
    MacroList m_list = ext.extension->get_macros();
    settings.interpreter.script_macros.insert(m_list.begin(),m_list.end());
    PreProcList p_list = ext.extension->get_preprocesses();
    settings.interpreter.script_preprocesses.insert(p_list.begin(),p_list.end());
    RawBuiltinList r_list = ext.extension->get_rawbuiltins();
    settings.interpreter.script_rawbuiltins.insert(r_list.begin(),r_list.end());

    settings.interpreter.extensions.push_back(ext);
    ext.extension->poke_interpreter(settings.interpreter);
    return true;
}

inline static bool bake_extension(const std::string& name, ScriptSettings& settings) noexcept {
    ExtensionData ext = get_ext(name);
    return bake_extension(ext,settings);
}

inline static std::string run_script(const std::string& source, ScriptSettings& settings) noexcept {
    auto labels = pre_process(source, settings);
    if(settings.error_msg != "") {
        return settings.error_msg;
    }
    settings.line = 1;
    std::string ret = run_label("main", labels, settings, std::filesystem::current_path().parent_path(), {});
    settings.exit = false;
    return ret;
}

inline static std::string run_label(const std::string& label_name, std::map<std::string, ScriptLabel> labels, ScriptSettings& settings, const std::filesystem::path& parent_path, const std::vector<ScriptVariable>& args) noexcept {
    if(labels.empty() || labels.count(label_name) == 0) return "";
    ScriptLabel label = labels[label_name];
    std::vector<lexed_kittens> lines;
    long long line = -1;
    for(auto i : label.lines) {
        if((long long)i.line != line) {
            line = i.line;
            lines.push_back({});
        }
        lines.back().push_back(i);
    }
    settings.label.push(label_name);
    for(auto i : lines) 
        if(i.size() != 2 || i[0].str || i[1].str || i[1].src.front() != '(') { 
            settings.label.pop();
            return "line " + std::to_string(i.front().line) + " is invalid (in label " + label_name + ")"; 
        }

    settings.parent_path = parent_path;
    settings.labels = labels;

    for(size_t i = 0; i < args.size(); ++i) {
        settings.variables[label.arglist[i]] = args[i];
    }
    if(settings.line == 0) settings.line = 1;
    for(size_t i = settings.line-1; i < lines.size(); ++i) {
        if(settings.exit) return "";
        std::string name = lines[i][0].src;
        if(settings.interpreter.has_rawbuiltin(name)) {
            ScriptRawBuiltin rawbuiltin = settings.interpreter.get_rawbuiltin(name);
            std::string r = lines[i][1].src;
            r.erase(r.begin());
            r.pop_back();

            rawbuiltin(r,settings);

            if(settings.error_msg != "") {
                settings.label.pop();
                if(settings.raw_error) return settings.error_msg;
                return "line " + std::to_string(settings.line + label.line) + ": " + settings.error_msg + " (in label " + label_name + ")";
            }
            i = settings.line - 1;
            ++settings.line;
            continue;
        }
        auto arglist = parse_argumentlist(lines[i][1].src, settings);
        if(settings.error_msg != "") {
            settings.label.pop();
            if(settings.raw_error) return settings.error_msg;
            return "line " + std::to_string(settings.line + label.line) + ": " + settings.error_msg + " (in label " + label_name + ")";
        }
        if(settings.interpreter.script_builtins.count(name) == 0) {
            settings.label.pop();
            return "line " + std::to_string(settings.line + label.line) + ": unknown function: " + name + " (in label " + label_name + ")";
        }
        ScriptBuiltin builtin = settings.interpreter.script_builtins[name];
        if(builtin.arg_count != (int)arglist.size() && builtin.arg_count >= 0) {
            settings.label.pop();
            return "line " + std::to_string(lines[i][0].line + label.line) + " " + name + " has invalid argument count " + " (in label " + label_name + ")";
        }
        builtin.exec(arglist,settings);
        if(settings.error_msg != "") {
            settings.label.pop();
            if(settings.raw_error) return settings.error_msg;
            return "line " + std::to_string(settings.line + label.line) + ": " + name + ": " + settings.error_msg + " (in label " + label_name + ")";
        }
        i = settings.line - 1;
        ++settings.line;
    }
    settings.label.pop();
    return "";
}

inline static bool is_operator_char(char c) noexcept {
    return c == '+' ||
           c == '-' ||
           c == '*' ||
           c == '/' ||
           c == '^' ||
           c == '%' ||
           c == '$' ||
           c == '|' ||
           c == '&' ||
           c == '~' ||
           c == '?' ||
           c == '!' ||
           c == '>' ||
           c == '<' ||
           c == '=';
}

inline static bool is_name_char(char c) noexcept {
    return (c <= 'Z' && c >= 'A') || 
            (c <= 'z' && c >= 'a') ||
            (c <= '9' && c >= '0') ||
            c == '_';
}

inline static bool is_name(const std::string& s) noexcept {
    if(s.empty()) return false;
    try { 
        std::stoi(std::string(1, s[0]));
        return false;
    }
    catch(...) {}
    for(auto i : s) {
        if(!is_name_char(i)) return false;
    }
    return true;
}

inline static bool is_label_arglist(std::string s) noexcept {
    if(s.size() < 2) return false;
    if(s[0] != '[' || s.back() != ']') return false;
    s.pop_back();
    s.erase(s.begin());

    KittenLexer arglist_lexer = KittenLexer()
        .add_extract(',')
        .ignore_backslash_opts()
        .erase_empty();
    auto lexed = arglist_lexer.lex(s);
    if(lexed.empty()) return true;

    bool found_n = true;
    for(auto i : lexed) {
        if(found_n) {
            if(!is_name(i.src)) return false;
            found_n = false;
        }
        else if(!found_n) {
            if(i.src != ",") return false;
            found_n = true;
        }
    }
    return !found_n;
}

inline static std::vector<std::string> parse_label_arglist(std::string s) noexcept {
    if(!is_label_arglist(s)) return {};
    s.pop_back();
    s.erase(s.begin());

    std::vector<std::string> ret;
    KittenLexer arglist_lexer = KittenLexer()
        .add_extract(',')
        .add_ignore(' ')
        .add_ignore('\t')
        .ignore_backslash_opts()
        .erase_empty();
    auto lexed = arglist_lexer.lex(s);

    bool found_n = true;
    for(auto i : lexed) {
        if(found_n) {
            if(!is_name(i.src)) return {};
            found_n = false;
            ret.push_back(i.src);
        }
        else if(!found_n) {
            if(i.src != ",") return {};
            found_n = true;
        }
    }
    return ret;
}

inline std::vector<ScriptVariable> parse_argumentlist(std::string source, ScriptSettings& settings) noexcept {
    source.erase(source.begin());
    source.pop_back();

    auto lexed = settings.interpreter.lexer.p_argumentlist(source);
    if(lexed.empty()) return std::vector<ScriptVariable>{};
    std::vector<std::string> args(1);
    for(auto i : lexed) {
        if(!i.str && i.src == ",") {
            args.push_back({});
        }
        else {
            if(i.str) i.src = "\"" + i.src + "\"";
            else if(settings.interpreter.script_macros.count(i.src) != 0) i.src = settings.interpreter.script_macros.at(i.src);

            args.back() += " " + i.src;
        }
    }

    std::vector<ScriptVariable> ret;
    for(auto i : args) {
        ret.push_back(evaluate_expression(i, settings));
        if(settings.error_msg != "") return {};
    }
    return ret;
}

inline static bool is_operator(const std::string& src, ScriptSettings& settings) noexcept {
    return settings.interpreter.script_operators.count(src) != 0;
}

struct _expressionErrors {
    std::vector<std::string> messages;
    bool has_new = false;
    
    inline _expressionErrors& push(std::string msg) noexcept {
        messages.push_back(msg);
        has_new = true;
        return *this;
    }

    inline bool changed() const noexcept { return has_new; }
    inline void reset() noexcept { has_new = false; }

    inline operator std::vector<std::string>() const noexcept { return messages; }
};
struct _expressionToken { std::string tk; ScriptOperator op; };
struct _expressionFuncall { 
    std::string function; 
    std::string arguments; 

    inline ScriptVariable call(ScriptSettings& settings, _expressionErrors& errors) noexcept {
        if(settings.interpreter.has_builtin(function)) {
            ScriptArglist args = parse_argumentlist(arguments, settings);
            ScriptBuiltin fun = settings.interpreter.get_builtin(function);
            if(settings.error_msg != "") {
                errors.push("error parsing argumentlist: " + settings.error_msg);
                settings.error_msg = "";
                return script_null;
            }
            if(fun.arg_count >= 0) {
                if((int)args.size() < fun.arg_count) {
                    errors.push("function call with too little arguments: " + function + arguments + 
                        "\n- needs: " + std::to_string(fun.arg_count) + " got: " + std::to_string(args.size()));
                    return script_null;
                }
                if((int)args.size() > fun.arg_count) {
                    errors.push("function call with too many arguments: " + function + arguments +
                        "\n- needs: " + std::to_string(fun.arg_count) + " got: " + std::to_string(args.size()));
                    return script_null;
                }
            }
            settings.error_msg = "";
            ScriptVariable ret =  fun.exec(args, settings);
            if(settings.error_msg != "") errors.push(settings.error_msg);
            return ret;
        }
        else {
            ScriptRawBuiltin rawbuiltin = settings.interpreter.get_rawbuiltin(function);
            std::string argcpy = arguments.substr(1, arguments.size() - 2);
            auto ret = rawbuiltin(argcpy, settings);
            if(settings.error_msg != "") {
                errors.push("error evaluating rawbuiltin \"" + function + "\": " + settings.error_msg);
                settings.error_msg = "";
                return script_null;
            }
            return ret;
        }
    }
};
struct _operatorToken { 
    ScriptVariable val;
    _expressionToken op;
    _expressionFuncall call;
    std::string capsule;
    enum { OP, VAL, CALL, CAPSULE } type;

    _operatorToken(ScriptVariable v): val(v) { type = VAL; }
    _operatorToken(_expressionToken v): op(v) { type = OP; }
    _operatorToken(_expressionFuncall v): call(v) { type = CALL; }
    _operatorToken(std::string v): capsule(v) { type = CAPSULE; }
    _operatorToken() = delete;

    inline ScriptVariable get_val(ScriptSettings& settings, _expressionErrors& errors) noexcept {
        switch(type) {
            case VAL:
                return val;
            case CALL:
                return call.call(settings, errors);
            case CAPSULE:
                {
                    std::string capsule_copy = capsule;
                    capsule_copy.erase(capsule_copy.begin());
                    capsule_copy.pop_back();
                    ScriptVariable value = evaluate_expression(capsule_copy, settings);
                    if(settings.error_msg != "") {
                        errors.push("Error while parsing " + capsule + ": " + settings.error_msg);
                        settings.error_msg = "";
                        return script_null;
                    }
                    return value;
                }
            case OP:
            default:
                break;
        }
        return script_null;
    }
};

inline static std::vector<_operatorToken> expression_prepare_tokens(lexed_kittens& tokens, ScriptSettings& settings, _expressionErrors& errors) noexcept {
    std::vector<_operatorToken> ret;
    for(size_t i = 0; i < tokens.size(); ++i) {
        auto token = tokens[i];
        std::string r = token.src;
        if(!token.str && is_operator(token.src, settings)) {
            ret.push_back(_expressionToken{r, ScriptOperator()});
        }
        else if(!token.str && token.src[0] == '(') {
            ret.push_back(token.src);
        }
        else if(!token.str && (settings.interpreter.has_builtin(token.src) || settings.interpreter.has_rawbuiltin(token.src))) {
            if(i + 1 >= tokens.size() || tokens[i + 1].str) {
                errors.push("function call without argument list");
                return {};
            }
            KittenToken arguments = tokens[i + 1];
            if(arguments.str || arguments.src.front() != '(') {
                errors.push("function call with invalid argument list: " + tokens[i].src + " " +tokens[i+1].src);
                return {};
            }

            ret.push_back(_expressionFuncall{tokens[i].src, tokens[i + 1].src});
            ++i;
        }
        else {
            ret.push_back(to_var(token, settings));
            if(is_null(ret.back().val)) {
                if(token.str) token.src = "\"" + token.src + "\"";
                errors.push("invalid literal: " + token.src);
            }
        }
    }

    return ret;
}

inline static ScriptVariable expression_check_prec(std::vector<_operatorToken> markedupTokens, int& state, const int& maxprec, ScriptSettings& settings, _expressionErrors& errors) noexcept {
    if(errors.changed()) return script_null;
    if(state >= (int)markedupTokens.size()) {
        errors.push("Unexpected end of expression");
        return script_null;
    }
    _operatorToken lhs = markedupTokens[state++];
        
    if(lhs.type == lhs.OP) {
        lhs = lhs.op.op.run(expression_check_prec(markedupTokens, state, lhs.op.op.priority, settings, errors), script_null,settings);
        if(settings.error_msg != "") {
            errors.push(settings.error_msg); 
            settings.error_msg = "";
        } 
        if(errors.changed()) return script_null;
    }

    while(state < (int)markedupTokens.size()) {
        _operatorToken vop = markedupTokens[state];
        if(vop.type != vop.OP) { 
            errors.push("expected operator: " + vop.val.printable());
            return script_null; 
        }
        auto op = vop.op.op;
        if(op.type != op.BINARY) { 
            errors.push("expected binary operator: " + vop.op.tk);
            return script_null; 
        }
        if(op.priority >= maxprec)
            break;
        ++state;
        ScriptVariable rhs = expression_check_prec(markedupTokens, state, op.priority, settings, errors);
        if(errors.changed()) return script_null;

        ScriptVariable old_lhs = lhs.get_val(settings, errors);
        if(errors.changed()) return script_null;
        lhs.type = lhs.VAL;
        lhs.val = op.run(old_lhs, rhs, settings);
        if(settings.error_msg != "") {
            errors.push(old_lhs.printable() + " " + vop.op.tk + " " + rhs.printable() + ": " + settings.error_msg);
            settings.error_msg = "";
            return script_null;
        }
    }
    return lhs.get_val(settings, errors);
}

inline static bool valid_expression(std::vector<_operatorToken> markedupTokens, ScriptSettings& settings) noexcept {
    const static int max_prec = 999999999;
    _expressionErrors errs;
    try {
        int i = 0;
        expression_check_prec(markedupTokens, i, max_prec, settings, errs);
    }
    catch(...) {
      return false;
    }
    return errs.changed();
}

inline static ScriptVariable expression_force_parse(std::vector<_operatorToken> markedupTokens, ScriptSettings& settings, _expressionErrors& errors, int i = 0) noexcept {
    const static int max_prec = 999999999;
    if(errors.changed()) return script_null;
    if(markedupTokens.size() == 1) {
        if(markedupTokens[0].type == markedupTokens[0].OP) {
            errors.push("standalone operator detected!");
            return script_null;
        }
        return markedupTokens[0].get_val(settings,errors);
    }
    if(i >= (int)markedupTokens.size()) {
        int i = 0;
        return expression_check_prec(markedupTokens, i, max_prec, settings, errors);
    }

    if(markedupTokens[i].type == markedupTokens[i].OP) {
        markedupTokens[i].op.op.type = ScriptOperator::UNARY;
    } 
    else {
        ++i;
        if(i >= (int)markedupTokens.size()) {
            int i = 0;
            return expression_check_prec(markedupTokens,i,max_prec,settings,errors); 
        }
        markedupTokens[i].op.op.type = ScriptOperator::BINARY;
    }

    for(auto option : settings.interpreter.script_operators[markedupTokens[i].op.tk]) {
        settings.error_msg = "";
        auto op = markedupTokens[i].op.op;
        if(option.type != op.type)
            continue;
        auto newMarkedupTokens = markedupTokens;
        newMarkedupTokens[i].op.op.priority = option.priority;
        newMarkedupTokens[i].op.op.run = option.run;
        auto test = expression_force_parse(newMarkedupTokens, settings, errors, i + 1);
        if(!is_null(test) && !errors.changed()) {
            return test;
        }
        errors.reset();
    }
    return script_null;
}

inline static ScriptVariable evaluate_expression(const std::string& source, ScriptSettings& settings) noexcept {
    auto lexed = settings.interpreter.lexer.p_expression(source);
    _expressionErrors errors;

    auto result = expression_force_parse(expression_prepare_tokens(lexed, settings, errors), settings, errors);

    if(errors.changed() || is_null(result)) {
        settings.error_msg = "\nError in expression: " + source + "\n";
        for(auto i : errors.messages) {
            settings.error_msg += i + "\n";
        }
        if(!errors.messages.empty()) settings.error_msg.pop_back();
        return script_null;
    }
    return result;
}

inline static void parse_const_preprog(const std::string& source, ScriptSettings& settings) noexcept {
    std::vector<lexed_kittens> lines;
    KittenLexer lexer = KittenLexer()
        .add_stringq('"')
        .add_capsule('(',')')
        .add_capsule('[',']')
        .add_capsule('{','}')
        .add_ignore(' ')
        .add_ignore('\t')
        .add_linebreak('\n')
        .add_lineskip('#')
        .add_con_extract(is_operator_char)
        .ignore_backslash_opts()
        .erase_empty();

    auto lexed = lexer.lex(source);
    long long line = -1;
    for(auto i : lexed) {
        if((long long)i.line != line) {
            line = i.line;
            lines.push_back({});
        }
        lines.back().push_back(i);
    }
    for(auto i : lines) {
        if(i.size() < 3 || i[0].str || i[1].str || !is_name(i[0].src) || i[1].src != "=") {
            settings.error_msg = "invalid syntax. <name> = <expression...>";
            return;
        }
        std::string name = i[0].src;
        std::string line;
        for(size_t j = 2; j < i.size(); ++j) {
            if(i[j].str) i[j].src = "\"" + i[j].src + "\"";
            line += i[j].src + " ";
        }
        line.pop_back();
        settings.constants[name] = evaluate_expression(line,settings);
        if(settings.error_msg != "") {
            return;
        }
    }
}

/**
 * Pre-processes the given source code and returns a map of script labels.
 *
 * @param source The source code to be pre-processed.
 * @param settings The settings for the script interpreter.
 *
 * @return A map of script labels.
 *
 * @throws None
 */
inline static std::map<std::string,ScriptLabel> pre_process(const std::string& source, ScriptSettings& settings) noexcept {
    std::map<std::string,ScriptLabel> ret;
    
    auto lexed = settings.interpreter.lexer.p_preprocess(source);
    std::vector<lexed_kittens> lines;
    long long line = -1;
    for(auto i : lexed) {
        if((long long)i.line != line) {
            line = i.line;
            lines.push_back({});
        }
        lines.back().push_back(i);
    }

    std::string current_label = "main";
    for(size_t i = 0; i < lines.size(); ++i) {
        auto& line = lines[i];
        if(line.size() != 0 && line[0].src == "@" && !line[0].str) {
            if(line.size() != 3 && (line.size() > 1 && line[1].src != "pragma")) {
                settings.error_msg = "line " + std::to_string(i+1) + ": invalid pre processor instruction: must have 2 arguments (got: " + std::to_string(line.size()-1) + ")";
                return {};
            }
            if(!is_name(line[1].src) || line[1].str) {
                settings.error_msg = "line " + std::to_string(i+1) + ": invalid pre processor instruction: expected instruction";
                return {};
            }
            std::string inst = line[1].src;

            if(inst == "const") {
                auto body = line[2].src;
                if(line[2].str) {
                    settings.error_msg = "line " + std::to_string(i+1) + ": const: unexpected string";
                    return {};
                }
                if(body.size() < 2) {
                    settings.error_msg = "line " + std::to_string(i+1) + ": const: unexpected token";
                    return {};
                }
                if(body.front() != '[' || body.back() != ']') {
                    settings.error_msg = "line " + std::to_string(i+1) + ": const: expected body";
                    return {};
                }
                body.erase(body.begin());
                body.erase(body.end() - 1);
                parse_const_preprog(body,settings);
                if(settings.error_msg != "") {
                    settings.error_msg = "line " + std::to_string(i+1) + ": const: line " + std::to_string(settings.line) + ": " + settings.error_msg;
                    return {};
                }
            }
            else if(inst == "bake") {
                KittenLexer bake_lexer = KittenLexer()
                    .add_stringq('"')
                    .erase_empty()
                    .add_ignore(' ')
                    .add_ignore('\t')
                    .add_ignore('\n')
                    ;
                auto body = line[2].src;
                if(line[2].str) {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": bake: unexpected string";
                    return {};
                }
                if(body.size() < 2) {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": bake: unexpected token";
                    return {};
                }
                if(body.front() != '[' || body.back() != ']') {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": bake: expected body";
                    return {};
                }
                body.erase(body.begin());
                body.erase(body.end()-1);
                auto lexed = bake_lexer.lex(body);
                for(auto b : lexed) {
                    if(!b.str) {
                        settings.error_msg = "line " + std::to_string(i + 1) + ": bake: expected value: " + b.src;
                        return {};
                    }
                    if(!bake_extension(b.src,settings)) {
                        settings.error_msg = "line " + std::to_string(i + 1) + ": bake: error baking extension: " + b.src + "\n"; 
                        return {};
                    }
                }
            }
            else if(inst == "pragma") {
                line.erase(line.begin(),line.begin() + 2);
                if(line.size() == 0) {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": pragma: no instruction";
                    return {};
                }
                if(line[0].str || settings.interpreter.script_preprocesses.count(line[0].src) == 0) {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": pragma: unknown instruction";
                    return {};
                }
                auto proc = settings.interpreter.script_preprocesses[line[0].src];
                line.erase(line.begin());
                proc(line,lines,i,settings);
                if(settings.error_msg != "") {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": pragma: " + settings.error_msg;
                    return {};
                }
            }
            else if(is_label_arglist(line[2].src) && !line[2].str) {
                if(ret.count(line[1].src) != 0) {
                    settings.error_msg = "line " + std::to_string(i + 1) + ": can't open label twice: " + line[1].src;
                    return {};
                }
                current_label = line[1].src;
                ret[current_label].arglist = parse_label_arglist(line[2].src);
                ret[current_label].line = line[1].line;
            }
            else {
                settings.error_msg = "line " + std::to_string(i + 1) + ": invalid pre processor instruction: no match for: " + inst;
                return {};
            }
        }
        else {
            for(auto j : line) ret[current_label].lines.push_back(j);
        }
    }

    return ret;
}

} /* namespace carescript */

#endif