#ifndef CARESCRIPT_PARSING_HPP
#define CARESCRIPT_PARSING_HPP

#include "carescript-defs.hpp"
#include "carescript-defaults.hpp"

#include <string.h>
#include <filesystem>

// Implementation for the functions declared in "carescript-defs.hpp"

namespace carescript {

///
//TODO add get_ext() for windows (ugh)
//TODO to_local_line(int) and to_global_line(int)
//TODO way to define rules using tags
///

#ifdef _WIN32
# include <windows.h>
inline static Extension* get_ext(std::string name) { return nullptr; }
#elif defined(__linux__)
# include <dlfcn.h>
inline static Extension* get_ext(std::string name) { 
    name = "." + CARESCRIPTSDK_DIRSLASH + name + ".so";
    void* handler = dlopen(name.c_str(),RTLD_NOW);
    if(handler == nullptr) return nullptr;
    get_extension_fun f = (get_extension_fun)dlsym(handler,"get_extension");
    if(f == nullptr) return nullptr;
    return f();
}
#endif

inline bool bake_extension(std::string name, ScriptSettings& settings) {
    Extension* ext = get_ext(name);
    return bake_extension(ext,settings);
}

inline bool bake_extension(Extension* ext, ScriptSettings& settings) {
    if(ext == nullptr) return false;

    BuiltinList b_list = ext->get_builtins();
    settings.interpreter.script_builtins.insert(b_list.begin(),b_list.end());
    TypeList t_list = ext->get_types();
    for(auto i : t_list) settings.interpreter.script_typechecks.push_back(i);
    OperatorList o_list = ext->get_operators();
    for(auto i : o_list) {
        for(auto j : i.second) {
            settings.interpreter.script_operators[i.first].push_back(j);
        }
    }
    MacroList m_list = ext->get_macros();
    settings.interpreter.script_macros.insert(m_list.begin(),m_list.end());
    return true;
}

inline std::string run_script(std::string source,ScriptSettings& settings) {
    auto labels = pre_process(source,settings);
    if(settings.error_msg != "") {
        return settings.error_msg;
    }
    std::string ret = run_label("main",labels,settings,std::filesystem::current_path().parent_path(),{});
    return ret;
}

inline std::string run_label(std::string label_name, std::map<std::string,ScriptLabel> labels, ScriptSettings& settings, std::filesystem::path parent_path, std::vector<ScriptVariable> args) {
    if(labels.empty() || labels.count(label_name) == 0) return "";
    ScriptLabel label = labels[label_name];
    std::vector<lexed_kittens> lines;
    int line = -1;
    for(auto i : label.lines) {
        if(i.line != line) {
            line = i.line;
            lines.push_back({});
        }
        lines.back().push_back(i);
    }
    settings.label.push(label_name);
    for(auto i : lines) 
        if(i.size() != 2 || i[0].str || i[1].str || i[1].src.front() != '(') { 
            settings.label.pop();
            return "line " + std::to_string(i.front().line-1 + label.line) + " is invalid (in label " + label_name + ")"; 
        }

    settings.parent_path = parent_path;
    settings.labels = labels;

    for(size_t i = 0; i < args.size(); ++i) {
        settings.variables[label.arglist[i]] = args[i];
    }
    if(settings.line == 0) settings.line = 1;
    for(size_t i = settings.line-1; i < lines.size(); ++i) {
        if(settings.exit) return "";
        i = settings.line-1;
        std::string name = lines[i][0].src;
        auto arglist = parse_argumentlist(lines[i][1].src,settings);
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
        if(builtin.arg_count != arglist.size() && builtin.arg_count >= 0) {
            settings.label.pop();
            return "line " + std::to_string(lines[i][0].line + label.line) + " " + name + " has invalid argument count " + " (in label " + label_name + ")";
        }
        builtin.exec(arglist,settings);
        if(settings.error_msg != "") {
            settings.label.pop();
            if(settings.raw_error) return settings.error_msg;
            return "line " + std::to_string(settings.line + label.line) + ": " + name + ": " + settings.error_msg + " (in label " + label_name + ")";
        }
        ++settings.line;
    }
    settings.label.pop();
    return "";
}

inline static bool is_operator_char(char c) {
    return c == '+' ||
           c == '-' ||
           c == '*' ||
           c == '/' ||
           c == '^' ||
           c == '%' ||
           c == '$';
}

inline static bool is_name_char(char c) {
    return (c <= 'Z' && c >= 'A') || 
            (c <= 'z' && c >= 'a') ||
            (c <= '9' && c >= '0') ||
            c == '_';
}

inline static bool is_name(std::string s) {
    if(s.empty()) return false;
    try { 
        std::stoi(std::string(1,s[0]));
        return false;
    }
    catch(...) {}
    for(auto i : s) {
        if(!is_name_char(i)) return false;
    }
    return true;
}

inline static bool is_label_arglist(std::string s) {
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

inline static std::vector<std::string> parse_label_arglist(std::string s) {
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

inline std::vector<ScriptVariable> parse_argumentlist(std::string source, ScriptSettings& settings) {
    KittenLexer arg_lexer = KittenLexer()
        .add_capsule('(',')')
        .add_capsule('[',']')
        .add_capsule('{','}')
        .add_stringq('"')
        .add_ignore(' ')
        .add_ignore('\t')
        .add_ignore('\n')
        .ignore_backslash_opts()
        .add_con_extract(is_operator_char)
        .add_extract(',')
        .erase_empty();

    source.erase(source.begin());
    source.pop_back();

    auto lexed = arg_lexer.lex(source);
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
        ret.push_back(evaluate_expression(i,settings));
        if(settings.error_msg != "") return {};
    }
    return ret;
}

inline static bool is_operator(std::string src, ScriptSettings& settings) {
    return settings.interpreter.script_operators.count(src) != 0;
}

inline static void process_op(std::stack<ScriptVariable>& st, std::vector<ScriptOperator> ops, ScriptSettings& settings) {
    ScriptVariable v;
    ScriptVariable r = st.top(); st.pop();
    ScriptVariable l = script_null;
    std::vector<std::string> error_msgs;
    if(st.empty()) {
        l = r;
        r = script_null;
    }
    else l = st.top();
    for(int i = 0; i < ops.size(); ++i) {
        ScriptOperator op = ops[i];
        if(op.type == op.UNARY) {
            v = op.run_unary(l,settings);
            if(!is_null(v)) {
                st.push(v);
                return;
            }
            else {
                error_msgs.push_back(settings.error_msg);
                settings.error_msg = "";
            }
        } 
        else {
            if(r == script_null) continue;
            ScriptVariable l = st.top();
            v = op.run(l,r,settings);
            if(!is_null(v)) {
                st.pop();
                st.push(v);
                return;
            }
            else {
                error_msgs.push_back(settings.error_msg);
                settings.error_msg = "";
            }
        }
    }
    settings.error_msg = "undefined operator for " + l.get_type() + " and " + r.get_type() + "\nOccured errors:\n";
    for(size_t i = 0; i < error_msgs.size(); ++i) {
        settings.error_msg += "Overload " + std::to_string(i+1) + ": " + error_msgs[i] + "\n";
    }
}

inline ScriptVariable evaluate_expression(std::string source, ScriptSettings& settings) {
    KittenLexer expression_lexer = KittenLexer()
        .add_stringq('"')
        .add_capsule('(',')')
        .add_con_extract(is_operator_char)
        .add_ignore(' ')
        .add_ignore('\t')
        .add_backslashopt('t','\t')
        .add_backslashopt('n','\n')
        .add_backslashopt('r','\r')
        .add_backslashopt('\\','\\')
        .add_backslashopt('"','\"')
        .erase_empty();
    auto lexed = expression_lexer.lex(source);

    ScriptVariable ret;

    std::stack<ScriptVariable> stack;
    std::stack<std::vector<ScriptOperator>> ops;

    bool may_be_unary = true;
    for(size_t i = 0; i < lexed.size(); ++i) {
        if(!lexed[i].str && is_operator(lexed[i].src,settings)) {
            std::vector<ScriptOperator> opss = settings.interpreter.script_operators[lexed[i].src];
            ScriptOperator cur_op = opss[0];
            if(may_be_unary && cur_op.type != cur_op.BOTH) cur_op.type = cur_op.UNARY;
            else cur_op.type = cur_op.DOUBLE;
            while (!ops.empty() && (
                    (cur_op.type == cur_op.DOUBLE && ops.top()[0].priority >= cur_op.priority) ||
                    (cur_op.type == cur_op.UNARY && ops.top()[0].priority > cur_op.priority)
                )) {
                process_op(stack, ops.top(),settings);
                if(settings.error_msg != "") return script_null;
                ops.pop();
            }
            ops.push(opss);
            may_be_unary = true;
        } 
        else {
            if(!lexed[i].str && lexed[i].src.front() == '(' && 
                !stack.empty() && is_typeof<ScriptNameValue>(stack.top())
                && settings.interpreter.script_builtins.count(get_value<ScriptNameValue>(stack.top())) != 0) {
                
                auto builtin = settings.interpreter.script_builtins[get_value<ScriptNameValue>(stack.top())];
                stack.pop();
                auto r = builtin.exec(parse_argumentlist(lexed[i].src,settings),settings);
                if(settings.error_msg != "") { 
                    return script_null;
                }
                stack.push(r);
            }
            else {
                if(!valid_literal(lexed[i],settings)) {
                    settings.error_msg = "invalid literal: " + lexed[i].src;
                    return script_null;
                }
                stack.push(to_var(lexed[i],settings));
            }
            may_be_unary = false;
        }
    }

    while (!ops.empty()) {
        process_op(stack, ops.top(),settings);
        if(settings.error_msg != "") return script_null;
        ops.pop();
    }
    if(stack.size() != 1) {
        settings.error_msg = "invalid expression: \"" + source + "\"";
        return script_null; 
    }
    return stack.top();
}

inline void parse_const_preprog(std::string source, ScriptSettings& settings) {
    std::vector<lexed_kittens> lines;
    KittenLexer lexer = KittenLexer()
        .add_stringq('"')
        .add_capsule('(',')')
        .add_capsule('[',']')
        .add_ignore(' ')
        .add_ignore('\t')
        .add_linebreak('\n')
        .add_lineskip('#')
        .add_con_extract(is_operator_char)
        .ignore_backslash_opts()
        .erase_empty();

    auto lexed = lexer.lex(source);
    int line = -1;
    for(auto i : lexed) {
        if(i.line != line) {
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

inline std::map<std::string,ScriptLabel> pre_process(std::string source, ScriptSettings& settings) {
    std::map<std::string,ScriptLabel> ret;
    KittenLexer lexer = KittenLexer()
        .add_stringq('"')
        .add_capsule('(',')')
        .add_capsule('[',']')
        .add_ignore(' ')
        .add_ignore('\t')
        .add_linebreak('\n')
        .add_lineskip('#')
        .add_extract('@')
        .ignore_backslash_opts()
        .erase_empty();
    
    auto lexed = lexer.lex(source);
    std::vector<lexed_kittens> lines;
    int line = -1;
    for(auto i : lexed) {
        if(i.line != line) {
            line = i.line;
            lines.push_back({});
        }
        lines.back().push_back(i);
    }

    std::string current_label = "main";
    for(size_t i = 0; i < lines.size(); ++i) {
        auto& line = lines[i];
        if(line.size() != 0 && line[0].src == "@" && !line[0].str) {
            if(line.size() != 3) {
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
                body.erase(body.end()-1);
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
                    settings.error_msg = "line " + std::to_string(i+1) + ": bake: unexpected string";
                    return {};
                }
                if(body.size() < 2) {
                    settings.error_msg = "line " + std::to_string(i+1) + ": bake: unexpected token";
                    return {};
                }
                if(body.front() != '[' || body.back() != ']') {
                    settings.error_msg = "line " + std::to_string(i+1) + ": bake: expected body";
                    return {};
                }
                body.erase(body.begin());
                body.erase(body.end()-1);
                auto lexed = bake_lexer.lex(body);
                for(auto b : lexed) {
                    if(!b.str) {
                        settings.error_msg = "line " + std::to_string(i+1) + ": bake: expected value: " + b.src;
                        return {};
                    }
                    if(!bake_extension(b.src,settings)) {
                        settings.error_msg = "line " + std::to_string(i+1) + ": bake: error baking extension: " + b.src + "\n"; 
                        return {};
                    }
                }
            }
            else if(is_label_arglist(line[2].src) && !line[2].str) {
                if(ret.count(line[1].src) != 0) {
                    settings.error_msg = "line " + std::to_string(i+1) + ": can't open label twice: " + line[1].src;
                }
                current_label = line[1].src;
                ret[current_label].arglist = parse_label_arglist(line[2].src);
                ret[current_label].line = line[1].line;
            }
            else {
                settings.error_msg = "line " + std::to_string(i+1) + ": invalid pre processor instruction: no match for: " + inst;
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