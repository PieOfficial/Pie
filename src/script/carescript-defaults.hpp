#ifndef CARESCRIPT_DEFAULTS_HPP
#define CARESCRIPT_DEFAULTS_HPP

#include "carescript-defs.hpp"



#ifdef _WIN32
#include <windows.h>
#include <lua.hpp>

#endif
#include "LuaManager.h"

/*
    Defines some default builtins, operators etc
    So it can be used out of the box
*/

namespace carescript {


inline std::map<std::string,ScriptBuiltin> default_script_builtins = {
    {"set",{2,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNameValue);
        cc_builtin_var_not_requires(args[1], ScriptNameValue);
        settings.variables[get_value<ScriptNameValue>(args[0])] = args[1];
        return script_null;
    }}},
    {"if",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        if(!settings.should_run.empty() && !settings.should_run.top()) {
            ++settings.ignore_endifs;
            return script_null; 
        }
        
        cc_builtin_var_requires(args[0], ScriptNumberValue);
        
        settings.should_run.push(get_value<ScriptNumberValue>(args[0]) == true);
        return script_null;
    }}},
    {"else",{0,[](const ScriptArglist&, ScriptSettings& settings) -> ScriptVariable {
        if(settings.should_run.empty()) {
            _cc_error("no if");
        }
        settings.should_run.top() = !settings.should_run.top();
        return script_null;
    }}},
    {"endif",{0,[](const ScriptArglist&, ScriptSettings& settings) -> ScriptVariable {
        if(settings.ignore_endifs != 0) {
            --settings.ignore_endifs;
            return script_null;
        }
        if(settings.should_run.empty()) _cc_error("no if");
        settings.should_run.pop();
        return script_null;
    }}},
    
    {"echo",{-1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        for(auto i : args) {
            std::cout << i.printable();
        }
        return script_null;
    }}},
    {"echoln",{-1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        for(auto i : args) {
            std::cout << i.printable();
        }
        std::cout << "\n";
        return script_null;
    }}},
    {"input",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptStringValue);
        std::cout << get_value<ScriptStringValue>(args[0]); std::cout.flush();
        std::string inp;
        std::getline(std::cin,inp);
        return new ScriptStringValue(inp);
    }}},

    {"to_number",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNumberValue, ScriptStringValue);

        // If the argument is already a number, return it as is
        if(is_typeof<ScriptNumberValue>(args[0])) return args[0];

        // If the argument is a string, try to convert it to a number
        long double num = 0;
        std::string s = get_value<ScriptStringValue>(args[0]);
        try {
            // Attempt to convert the string to a long double
            std::size_t idx = 0;
            num = std::stold(s, &idx);

            // If the conversion is not successful (some characters are not part of the number),
            // throw an exception to handle the error
            if(idx != s.size()) throw 0;
        }
        catch(...) {
            // Catch any exceptions thrown during conversion and report an error
            _cc_error("invalid input: \"" + s + "\"");
        }

        // Return the converted number as a ScriptNumberValue
        return new ScriptNumberValue(num);
    }}},
    {"to_string",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNumberValue, ScriptStringValue);
        if(is_typeof<ScriptNumberValue>(args[0])) {
            return new ScriptStringValue(std::to_string(get_value<ScriptNumberValue>(args[0])));
        }
        else if(is_typeof<ScriptStringValue>(args[0])) {
            return args[0];
        }

        return script_null;
    }}},

    {"exec",{-1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        if(args.size() < 2) {
            _cc_error("requires at least two arguments");
        }
        cc_builtin_var_requires(args[0], ScriptStringValue);
        std::string f;
        std::ifstream ifile;
        ifile.open(get_value<ScriptStringValue>(args[0]), std::ios::in);
        while(ifile.good()) f += ifile.get();
        if(!f.empty()) f.pop_back();
        ifile.close();

        std::string label = get_value<ScriptStringValue>(args[1]);
        auto args2 = args;
        args2.erase(args.begin(),args.begin() + 1);
        args2.erase(args.begin(),args.begin() + 1);

        Interpreter interp;
        interp.script_builtins = settings.interpreter.script_builtins;
        interp.script_operators = settings.interpreter.script_operators;
        interp.script_typechecks = settings.interpreter.script_typechecks;
        interp.script_macros = settings.interpreter.script_macros;
        interp.script_rawbuiltins = settings.interpreter.script_rawbuiltins;
        interp.pre_process(f).on_error([&](Interpreter& i) {
            settings.error_msg = i.error();
        });
        if(settings.error_msg != "") return script_null;
        return interp.run(label, args2).on_error([&](Interpreter& i) {
            settings.error_msg = i.error();
        }).get_value_or(script_null);
    }}},
    {"exit",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNumberValue);
        std::exit((int)get_value<ScriptNumberValue>(args[0]));
        return script_null;
    }}},
    {"system",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptStringValue);
        system((get_value<ScriptStringValue>(args[0])).c_str());
        return script_null;
    }}},

    /*
    {"add",{2,[](const ScriptArglist& args, ScriptSettings& settings)->ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0],ScriptNameValue);
        cc_builtin_var_requires(args[1],ScriptStringValue);
        std::string type = get_value<ScriptNameValue>(args[0]);
        std::string file = get_value<ScriptStringValue>(args[1]);
        
        if(type == "FILE") {
            std::ofstream ofile(file);
            ofile.close();
        }
        else if(type == "DIRECTORY") {
            std::filesystem::create_directory(file);
        }
        else {
            _cc_error("unknown enum: " + type);
        }

        return script_null;
    }}},
    {"remove",{2,[](const ScriptArglist& args, ScriptSettings& settings)->ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0],ScriptNameValue);
        cc_builtin_var_requires(args[1],ScriptStringValue);
        std::string type = get_value<ScriptNameValue>(args[0]);
        std::string path = get_value<ScriptStringValue>(args[1]);

        in_glob_expand(file,path) {
            if(type == "FILE") {
                try {
                    std::filesystem::remove(file);
                }
                catch(...) {
                    _cc_error("no such file");
                }
            }
            else if(type == "DIRECTORY") {
                try {
                    std::filesystem::remove_all(file);
                }
                catch(...) {
                    _cc_error("no such directory");
                }
            }
            else {
                _cc_error("unknown enum: " + type);
            }
        }

        return script_null;
    }}},
    {"exists",{2,[](const ScriptArglist& args, ScriptSettings& settings)->ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0],ScriptNameValue);
        cc_builtin_var_requires(args[1],ScriptStringValue);
        std::string type = get_value<ScriptNameValue>(args[0]);
        std::string path = get_value<ScriptStringValue>(args[1]);

        in_glob_expand(file,path) {
            if(type == "FILE") {
                if(std::filesystem::exists(file)) return script_true;
            }
            else if(type == "DIRECTORY") {
                if(std::filesystem::exists(file)) return script_true;
            }
            else if(type == "VARIABLE") {
                if(settings.variables.count(file) != 0) return script_true;
            }
            else {
                _cc_error("unknown enum: " + type);
            }
        }
        return script_false;
    }}},
    {"copy",{2,[](const ScriptArglist& args, ScriptSettings& settings)->ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0],ScriptStringValue);
        cc_builtin_var_requires(args[1],ScriptStringValue);
        std::string path = get_value<ScriptStringValue>(args[0]);
        std::string dest_ = get_value<ScriptStringValue>(args[1]);

        in_glob_expand(file,path) {
            std::string dest = dest_;
            if(std::filesystem::is_directory(path)) {
                dest += CARESCRIPTSDK_DIRSLASH + file.string(); 
            }
            if(!std::filesystem::exists(file)) {
                _cc_error("no such file: " + file.string());
            }
            if(std::filesystem::exists(dest)) {
                std::filesystem::remove_all(dest);
            }
            std::filesystem::copy(file,dest);
        }
        return script_null;
    }}},
    */

    {"read",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptStringValue);
        std::string r;
        std::ifstream ifile;
        ifile.open(get_value<ScriptStringValue>(args[0]));
        while(ifile.good()) r += ifile.get();
        if(!r.empty()) r.pop_back();

        return new ScriptStringValue{r};
    }}},
    {"write",{2,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptStringValue);
        cc_builtin_var_requires(args[1], ScriptStringValue);
        std::ofstream ofile;
        ofile.open(get_value<ScriptStringValue>(args[0]), std::ios::trunc);
        ofile.close(); ofile.open(get_value<ScriptStringValue>(args[0]));
        ofile << get_value<ScriptStringValue>(args[1]);
        ofile.close();
        return script_null;
    }}},
    
    {"call",{-1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        if(args.size() == 0) {
            _cc_error("requires at least one argument");
        }
        cc_builtin_var_requires(args[0], ScriptNameValue);
        std::vector<ScriptVariable> run_args;
        for(size_t i = 1; i < args.size(); ++i) {
            // cc_builtin_var_requires(i,ScriptNameValue);
            run_args.push_back(args[i]);
        }
        
        if(settings.labels.count(get_value<ScriptNameValue>(args[0])) == 0) {
            _cc_error("no such label " + get_value<ScriptNameValue>(args[0]));
        }
        ScriptLabel label = settings.labels[get_value<ScriptNameValue>(args[0])];
        if(label.arglist.size() > run_args.size()) {
            _cc_error("too few arguments");
        }
        else if(label.arglist.size() < run_args.size()) {
            _cc_error("too many arguments");
        }
        ScriptSettings tset(settings.interpreter);
        tset.constants = settings.constants;
        settings.error_msg = run_label(get_value<ScriptNameValue>(args[0]), settings.labels, tset, "", run_args);
        if(settings.error_msg != "") settings.raw_error = true;

        return tset.return_value;
    }}},
    {"return",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        settings.return_value = args[0];
        settings.exit = true;
        return script_null;
    }}},

    {"strmod",{-1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        if(args.size() < 2) {
            _cc_error("requires at least two argument");
        }
        cc_builtin_var_requires(args[0], ScriptNameValue);
        cc_builtin_var_requires(args[1], ScriptNameValue);
        
        ScriptVariable vr = settings.variables[get_value<ScriptNameValue>(args[1])];
        if(is_typeof<ScriptStringValue>(vr)) {
            _cc_error("requires string variable");
        }
        std::string str = get_value<ScriptStringValue>(vr);

        if(get_value<ScriptNameValue>(args[0]) == "ERASE") {
            if(args.size() != 3) _cc_error("requires 3 arguments");
            cc_builtin_var_requires(args[2], ScriptNumberValue);
            auto idx = get_value<ScriptNumberValue>(args[2]);
            if(str.empty()) _cc_error("string empty");
            if(idx >= str.size()) _cc_error("index overflow");
            if(idx < 0) _cc_error("index undeflow");

            str.erase(str.begin()+idx);
            settings.variables[get_value<ScriptNameValue>(args[1])] = new ScriptStringValue(str);
        }
        else if(get_value<ScriptNameValue>(args[0]) == "INSERT") {
            if(args.size() != 4) _cc_error("requires 4 arguments");
            cc_builtin_var_requires(args[2], ScriptNumberValue);
            cc_builtin_var_requires(args[3], ScriptStringValue);
            auto idx = get_value<ScriptNumberValue>(args[2]);
            if(str.empty()) _cc_error("string empty");
            if(idx >= str.size()) _cc_error("index overflow");
            if(idx < 0) _cc_error("index undeflow");

            str = str.substr(0,idx-1) + get_value<ScriptStringValue>(args[3]) + str.substr(idx,str.size()-1);
            settings.variables[get_value<ScriptNameValue>(args[1])] = new ScriptStringValue(str);
        }
        else if(get_value<ScriptNameValue>(args[0]) == "PUT") {
            if(args.size() != 4) _cc_error("requires 4 arguments");
            cc_builtin_var_requires(args[2], ScriptNumberValue);
            cc_builtin_var_requires(args[3], ScriptStringValue);
            auto idx = get_value<ScriptNumberValue>(args[2]);
            if(str.empty()) _cc_error("string empty");
            if(idx >= str.size()) _cc_error("index overflow");
            if(idx < 0) _cc_error("index undeflow");
            
            str = str.substr(0,idx) + get_value<ScriptStringValue>(args[3]) + str.substr(idx+1,str.size()-1);
            settings.variables[get_value<ScriptNameValue>(args[1])] = new ScriptStringValue(str);
        }
        else if(get_value<ScriptNameValue>(args[0]) == "BACK") {
            if(args.size() != 2) _cc_error("requires 2 arguments");
            if(str.empty()) _cc_error("string empty");

            return new ScriptStringValue(std::string(1,str.back()));
        }
        else if(get_value<ScriptNameValue>(args[0]) == "SIZE") {
            if(args.size() != 2) _cc_error("requires 2 arguments"); 

            return new ScriptNumberValue((long double)str.size());
        }
        else if(get_value<ScriptNameValue>(args[0]) == "AT") {
            if(args.size() != 3) _cc_error("requires 3 arguments");
            cc_builtin_var_requires(args[2], ScriptNumberValue);
            if(str.empty()) _cc_error("string empty");
            auto idx = get_value<ScriptNumberValue>(args[2]);
            if(idx >= str.size()) _cc_error("index overflow");
            if(idx < 0) _cc_error("index undeflow");

            return new ScriptStringValue(std::string(1,str.at(idx)));
        }
        else if(get_value<ScriptNameValue>(args[0]) == "SUBSTR") {
            if(args.size() != 4) _cc_error("requires 4 arguments");
            cc_builtin_var_requires(args[2], ScriptNumberValue);
            cc_builtin_var_requires(args[3], ScriptNumberValue);
            if(str.empty()) _cc_error("string empty");
            auto idx_from = get_value<ScriptNumberValue>(args[2]);
            auto idx_to = get_value<ScriptNumberValue>(args[3]);
            if(idx_from >= str.size()) _cc_error("index overflow");
            if(idx_from < 0) _cc_error("index undeflow");
            if(idx_to >= str.size()) _cc_error("index overflow");
            if(idx_to < 0) _cc_error("index undeflow");
            if(idx_to < idx_from) {int t = idx_to; idx_to = idx_from; idx_from = t;}

            return new ScriptStringValue(str.substr(idx_to, idx_from - idx_to));
        }
        else {
            _cc_error("unknown enum type");
        }

        return script_null;
    }}},

    {"bake",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptStringValue);
        return bake_extension(get_value<ScriptStringValue>(args[0]),settings) ? script_true : script_false;
    }}},
    {"typeof",{1,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        return new ScriptStringValue(args[0].get_type());
    }}},

    {"beep",{2,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNumberValue);
        cc_builtin_var_requires(args[1], ScriptNumberValue);

        // Check if the code is being compiled on a Windows platform
        #ifdef _WIN32
        // If on Windows, use the Beep function to generate a system beep
        Beep((int)get_value<ScriptNumberValue>(args[0]), (int)get_value<ScriptNumberValue>(args[1]));
        #endif

        return script_null;
    }}},
    {"exists",{2,[](const ScriptArglist& args, ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNameValue);
        cc_builtin_var_requires(args[1], ScriptStringValue);

        std::string type = get_value<ScriptNameValue>(args[0]);
        std::string path = get_value<ScriptStringValue>(args[1]);

        std::filesystem::path file(path);

        if (type == "FILE" || type == "DIRECTORY") {
            if (std::filesystem::exists(file)) {
                return script_true;
            }
        } else if (type == "VARIABLE") {
            if (settings.variables.count(path) != 0) {
                return script_true;
            }
        } else {
            _cc_error("unknown enum: " + type);
        }

        return script_false;
    }}},
    // usage: push(list,to_push1,to_push2,...)
    {"push",{-1,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        // Note how we can already use "ListType" here as any other type
        cc_builtin_var_requires(args[0], ListType);
        cc_builtin_arg_min(args, 2);
        ListType list = get_value<ListType>(args[0]);
        for(size_t i = 1; i < args.size(); ++i) {
            list.list.push_back(args[i]);
        }
        return new ListType(list);
    }}},
    // usage: pop(list,[times])
    {"pop",{-1,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ListType);
        cc_builtin_arg_range(args, 1, 2);
        size_t count = 1;
        if(args.size() == 2) {
            cc_builtin_var_requires(args[1], ScriptNumberValue);
            count = (size_t)get_value<ScriptNumberValue>(args[1]);
        }
        ListType list = get_value<ListType>(args[0]);
        _cc_error_if(list.list.size() <= count,"popped not existing element (size below 0)");
        for(size_t i = 0; i < count; ++i) list.list.pop_back();
        return new ListType(list);
    }}},
    {"list_size",{1,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ListType);
        ListType list = get_value<ListType>(args[0]);
        return new ScriptNumberValue(list.list.size());
    }}},
    {"deref",{1,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ReferenceType);
        ReferenceType ref = get_value<ReferenceType>(args[0]);
        return ref.get_value()->value->copy();
    }}},
    {"ref",{1,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptNameValue);
        ScriptVariable& var = settings.variables[get_value<ScriptNameValue>(args[0])];
        return new ReferenceType(var);
    }}},
    {"setref",{2,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ReferenceType);
        ScriptVariable* var = get_value<ReferenceType>(args[0]);
        *var = args[1];
        return script_null;
    }}},
    {"call_lua_script",{1,[](const ScriptArglist& args,ScriptSettings& settings) -> ScriptVariable {
        cc_builtin_if_ignore();
        cc_builtin_var_requires(args[0], ScriptStringValue);
        const char* script_name = get_value<ScriptStringValue>(args[0]).c_str();
        lua_State* L = LuaManager::getInstance().getState();
        // Load the Lua script
        if (luaL_loadfile(L, script_name) != LUA_OK) {
            _cc_error(std::string("Error loading script: ") + lua_tostring(L, -1));
            return 1;
        }

        // 4. Execute the script
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
            std::cerr << "Error running script: " << lua_tostring(L, -1) << std::endl;
            return 1;
        }

        // 5. Get the output from the script
        int numResults = lua_gettop(L);  // Get the number of results on the stack

        if (numResults > 0) {
            std::cout << "Script output:" << std::endl;
            for (int i = 1; i <= numResults; ++i) {
                // Check the type of each result and print it accordingly
                if (lua_isstring(L, i)) {
                    std::cout << lua_tostring(L, i) << std::endl;
                    return new ScriptStringValue(lua_tostring(L, i));
                } else if (lua_isnumber(L, i)) {
                    std::cout << lua_tonumber(L, i) << std::endl;
                } else {
                    // Handle other types as needed
                    std::cout << "Unknown result type" << std::endl;
                }
            }
        }

        // 6. Close the Lua state
        //lua_close(L);

        return script_null;
    }}},
    // TODO: implement function that deletes a variable
};

inline std::vector<ScriptTypeCheck> default_script_typechecks = {
    [](KittenToken src,ScriptSettings&) -> ScriptValue* {
        if(src.str) {
            return new ScriptStringValue(src.src);
        }
        return nullptr;
    },
    [](KittenToken src,ScriptSettings&) -> ScriptValue* {
        if(src.str) return nullptr;
        try {
            return new ScriptNumberValue(std::stold(src.src));
        }
        catch(...) {}
        return nullptr;
    },
    [](KittenToken src,ScriptSettings&) -> ScriptValue* {
        if(src.str) return nullptr;
        if(src.src == "null") return new ScriptNullValue();
        return nullptr;
    },
    [](KittenToken src,ScriptSettings&) -> ScriptValue* {
        if(src.str || src.src.empty()) return nullptr;
        try { 
            std::stoi(std::string(1, src.src[0]));
            return nullptr;
        }
        catch(...) {}
        for(auto c : src.src) {
            if(!((c <= 'Z' && c >= 'A') || 
            (c <= 'z' && c >= 'a') ||
            (c <= '9' && c >= '0') ||
            c == '_')) return nullptr;
        }
        return new ScriptNameValue(src.src);
    },
    [](KittenToken src, ScriptSettings& settings) -> ScriptValue* {
        if(src.str) return nullptr; // on error, return a nullptr
        KittenLexer lx = KittenLexer()
        .add_capsule('[',']')
        .add_ignore(',') // set commas as ignored
        .add_ignore(' ') // set whitespaces as ignored
        .erase_empty()
        .add_stringq('"');
        auto lex1 = lx.lex(src.src);
        if(lex1.size() != 1) return nullptr;
        lex1[0].src.erase(lex1[0].src.begin());
        lex1[0].src.erase(lex1[0].src.end() - 1);

        auto plist = lx.lex(lex1[0].src);
        ListType list;
        for(auto i : plist) {
            if(i.str) i.src = "\"" + i.src + "\"";
            list.list.push_back(evaluate_expression(i.src, settings));
        }
        return new ListType(list.list);
    },
};

inline std::map<std::string,std::vector<ScriptOperator>> default_script_operators = {
    {"+",{{0,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "+");
        cc_operator_var_requires(right, "+", ScriptNumberValue, ScriptStringValue);
        if(is_typeof<ScriptNumberValue>(right)) {
            return new ScriptNumberValue(
                    get_value<ScriptNumberValue>(left) + get_value<ScriptNumberValue>(right)
                );
        }
        else {
            return new ScriptStringValue(
                    get_value<ScriptStringValue>(left) + get_value<ScriptStringValue>(right)
                );
        }
    }}}},
    // adds a "+" operator to join lists together
    {"+",{{0,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "+");
        cc_operator_var_requires(right, "+", ListType);
        auto lvec = get_value<ListType>(left);
        auto rvec = get_value<ListType>(right);
        for(auto& i : rvec) lvec.push_back(i);
        return new ListType(lvec);
    }}}},
    {"-",{{0,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "-");
        cc_operator_var_requires(right, "-", ScriptNumberValue);
        ScriptVariable ret;
        ret = new ScriptNumberValue(
                get_value<ScriptNumberValue>(left) - get_value<ScriptNumberValue>(right)
            );
        return ret;
    }},{-3,ScriptOperator::UNARY,[](const ScriptVariable& left, const ScriptVariable&, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_var_requires(left, "-", ScriptNumberValue);
        ScriptVariable ret;
        ret = new ScriptNumberValue(
                get_value<ScriptNumberValue>(left) * -1
            );
        return ret;
    }}}},
    {"*",{{-1,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "*");
        cc_operator_var_requires(right, "*", ScriptNumberValue);
        ScriptVariable ret;
        ret = new ScriptNumberValue(
                get_value<ScriptNumberValue>(left) * get_value<ScriptNumberValue>(right)
            );

        return ret;
    }}}},
    {"/",{{-1,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "/");
        cc_operator_var_requires(right, "/", ScriptNumberValue);
        if(get_value<ScriptNumberValue>(right) == 0) {
            settings.error_msg = "/: division through 0 is not allowed";
            return script_null;
        }
        ScriptVariable ret;
        ret = new ScriptNumberValue(
                get_value<ScriptNumberValue>(left) / get_value<ScriptNumberValue>(right)
            );
        return ret;
    }}}},
    {"^",{{-2,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "^");
        cc_operator_var_requires(right, "^", ScriptNumberValue);
        ScriptVariable ret;
        ret = new ScriptNumberValue(
                std::pow(get_value<ScriptNumberValue>(left), get_value<ScriptNumberValue>(right))
            );
        return ret;
    }}}},
    
    {"is",{{2,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "is");

        return new ScriptNumberValue(
                left == right ? true : false
            );
    }}}},
    {"isnt",{{2,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_same_type(right, left, "isnt");

        return new ScriptNumberValue(
                left == right ? false : true
            );
    }}}},
    {"and",{{3,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings)->ScriptVariable {
        cc_operator_same_type(right, left, "and");
        cc_operator_var_requires(right, "and", ScriptNumberValue);
        
        return new ScriptNumberValue(
                (get_value<ScriptNumberValue>(left) == true && get_value<ScriptNumberValue>(right)) ? true : false
            );
    }}}},
    {"or",{{4,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings)->ScriptVariable {
        cc_operator_same_type(right, left, "or");
        cc_operator_var_requires(right, "or", ScriptNumberValue);
                
        return new ScriptNumberValue(
                (get_value<ScriptNumberValue>(left) == true || get_value<ScriptNumberValue>(right) == true) ? true : false
            );
    }}}},
    {"more",{{5,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings)->ScriptVariable {
        cc_operator_same_type(right, left, "more");
        cc_operator_var_requires(right, "more", ScriptNumberValue);
        return new ScriptNumberValue(
                (get_value<ScriptNumberValue>(left) > get_value<ScriptNumberValue>(right)) ? true : false
            );
    }}}},
    {"less",{{5,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings)->ScriptVariable {
        cc_operator_same_type(right, left, "less");
        cc_operator_var_requires(right, "less", ScriptNumberValue);
        return new ScriptNumberValue(
                (get_value<ScriptNumberValue>(left) < get_value<ScriptNumberValue>(right)) ? true : false
            );
    }}}},
    
    {"not",{{-4,ScriptOperator::UNARY,[](const ScriptVariable& left, const ScriptVariable&, ScriptSettings& settings)->ScriptVariable {
        cc_operator_var_requires(left, "not", ScriptNumberValue);
        return new ScriptNumberValue(
                !get_value<ScriptNumberValue>(left)
            );
    }}}},
    {"$",{{-5,ScriptOperator::UNARY,[](const ScriptVariable& left, const ScriptVariable&, ScriptSettings& settings) -> ScriptVariable {
        cc_operator_var_requires(left, "$", ScriptNameValue);
        ScriptVariable ret;
        std::string v = get_value<ScriptNameValue>(left);
        if(settings.variables.count(v) != 0) {
            return settings.variables[v];
        }
        else if(settings.constants.count(v) != 0) {
            return settings.constants[v];
        }
        settings.error_msg = "$: left is not a registered variable or constant!";
        return script_null; 
    }}}}, 
};

inline std::unordered_map<std::string, std::string> default_script_macros = {
#ifdef _WIN32
    {"WINDOWS","1"},
    {"LINUX","0"},
    {"UNKNOWN","0"},
    {"OSNAME","\"WINDOWS\""},
#elif defined(__linux__)
    {"WINDOWS","0"},
    {"LINUX","1"},
    {"UNKNOWN","0"},
    {"OSNAME","\"LINUX\""},
#elif
    {"WINDOWS","0"},
    {"LINUX","0"},
    {"UNKNOWN","1"},
    {"OSNAME","\"UNKNOWN\""},
#endif
};

} /* namespace carescript */

#endif