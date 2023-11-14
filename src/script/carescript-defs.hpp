#ifndef CARESCRIPT_DEFS_HPP
#define CARESCRIPT_DEFS_HPP

#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <filesystem>
#include <exception>
#include <functional>
#include <any>

#include "kittenlexer.hpp"

#include "carescript-types.hpp"
#include "carescript-macromagic.hpp"

namespace carescript {

template<typename Tp>
concept ScriptValueType = std::is_base_of<carescript::ScriptValue,Tp>::value;

// Wrapper class to perfom tasks on a
// subclass of the abstract class "ScriptValue"
struct ScriptVariable {
    std::unique_ptr<ScriptValue> value = nullptr;

    inline bool operator==(const ScriptVariable& sv) const noexcept {
        return *sv.value == *value;
    }
    
    ScriptVariable() {}
    ScriptVariable(ScriptValue* ptr): value(ptr) {}
    ScriptVariable(const ScriptVariable& var) {
        if(var.value.get() != nullptr) value.reset(var.value->copy());
    }

    template<typename Tp>
    ScriptVariable(Tp a) {
        // note here: we use `from` but it is class internally only
        // defined for ScriptValue pointers.
        // The user can define other `from` functions inside the
        // `carescript` namespace to effectivly overload the
        // constructor of this class, enabling syntax sugar for user defined types
        from(*this,a);
    }

    inline ScriptVariable& operator=(const ScriptVariable& var) noexcept {
        if(var.value.get() != nullptr) value.reset(var.value->copy());
        return *this;
    }

    inline std::string get_type() const noexcept {
        return value.get()->get_type();
    }
    inline std::string printable() const noexcept{
        return value.get()->to_printable();
    }
    inline std::string string() const noexcept {
        return value.get()->to_string();
    }

    template<typename Tp>
    inline operator Tp() const noexcept {
        return get_value<Tp>(*this);
    }

    inline friend void from(ScriptVariable& var, ScriptValue* a) noexcept {
        var.value.reset(a);
    }
};

// checks if a variable has a specific type
template<ScriptValueType Tval>
inline static bool is_typeof(const carescript::ScriptVariable& var) noexcept {
    Tval inst;
    return var.get_type() == inst.get_type();
}

// checks if two subclasses of ScriptValue are the same
template<ScriptValueType Tp1, ScriptValueType Tp2>
inline static bool is_same_type() noexcept {
    Tp1 v1;
    Tp2 v2;
    return v1.get_type() == v2.get_type();
}

// checks if two ScriptVariable instances have the same type
inline static bool is_same_type(const ScriptVariable& v1,const ScriptVariable& v2) noexcept {
    return v1.get_type() == v2.get_type();
}

// checks if a variable is null
inline static bool is_null(const ScriptVariable& v) noexcept {
    return v.value.get() == nullptr || is_typeof<ScriptNullValue>(v);
}

// returns the unwrapped type of a variable
template<typename Tp>
inline static auto get_value(const carescript::ScriptVariable& v) noexcept {
    return ((const Tp*)v.value.get())->get_value();
}
template<typename Tp>
inline static auto& get_value(carescript::ScriptVariable& v) noexcept {
    return ((Tp*)v.value.get())->get_value();
}

// modify only if you know what you're doing
inline static ScriptVariable script_null = new ScriptNullValue();
inline static ScriptVariable script_true = new ScriptNumberValue(true);
inline static ScriptVariable script_false = new ScriptNumberValue(false);

struct Interpreter;
struct ScriptLabel;
// general storage class for the current state of execution
struct ScriptSettings {
    Interpreter& interpreter;
    int line = 0;
    bool exit = false;
    std::stack<bool> should_run;
    std::map<std::string,ScriptVariable> variables;
    std::map<std::string,ScriptVariable> constants;
    std::map<std::string,ScriptLabel> labels;
    std::filesystem::path parent_path;
    int ignore_endifs = 0;
    ScriptVariable return_value = script_null;

    std::string error_msg;
    bool raw_error = false;
    std::stack<std::string> label;

    std::map<std::string,std::any> storage;

    ScriptSettings(Interpreter& i): interpreter(i) {}

    inline void clear() noexcept {
        line = 0;
        exit = false;
        should_run = std::stack<bool>();
        variables.clear();
        constants.clear();
        labels.clear();
        parent_path = "";
        ignore_endifs = 0;
        return_value = script_null;
        error_msg = "";
        raw_error = false;
        label = std::stack<std::string>();
        storage.clear();
    }
};

// storage class for an operator
struct ScriptOperator {
    // higher priority -> the later it gets evaluated
    int priority = 0;
    // UNKNOWN -> internally used, no no, don't touch it!
    enum {UNARY, BINARY, UNKNOWN} type;
    // if UNARY, `right` will always be script_null
    ScriptVariable(*run)(const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings) = nullptr;
};

using ScriptArglist = std::vector<ScriptVariable>;
// simple C-like replacement macro, not recursive
using ScriptMacro = std::pair<std::string,std::string>;
// instead of evaluated arguments, this get's the raw input
using ScriptRawBuiltin = ScriptVariable(*)(const std::string& source, ScriptSettings& settings);
// tries to evaluate a string into a variable, nullptr if fauled
using ScriptTypeCheck = ScriptValue*(*)(KittenToken src, ScriptSettings& settings);

// storage class for a builtin function
struct ScriptBuiltin {
    // < 0 disables this check
    int arg_count = -1;
    // return `script_null` for no return value
    ScriptVariable(*exec)(const ScriptArglist&, ScriptSettings&);
};

// storage class for a label
struct ScriptLabel {
    std::vector<std::string> arglist;
    lexed_kittens lines;
    int line = 0;
};

extern std::map<std::string,ScriptBuiltin> default_script_builtins;
extern std::map<std::string,std::vector<ScriptOperator>> default_script_operators;
extern std::vector<ScriptTypeCheck> default_script_typechecks;
extern std::unordered_map<std::string,std::string> default_script_macros;

using ScriptPreProcess = std::function<void(const lexed_kittens&,std::vector<lexed_kittens>&,size_t,ScriptSettings&)>;
extern std::unordered_map<std::string, ScriptPreProcess> default_script_preprocesses;

// bakes an extension into the interpreter
inline static bool bake_extension(const std::string& name, ScriptSettings& settings) noexcept;
struct ExtensionData;
class Extension;
inline static bool bake_extension(ExtensionData extension, ScriptSettings& settings) noexcept;

// runs a "main" function of a script
inline static std::string run_script(const std::string& source, ScriptSettings& settings) noexcept;
// runs a specific label with the given parameters
inline static std::string run_label(const std::string& label_name, std::map<std::string, ScriptLabel> labels, ScriptSettings& settings, const std::filesystem::path& parent_path, const std::vector<ScriptVariable>& args) noexcept;

// preprocesses the file into the interpreter
inline static std::map<std::string,ScriptLabel> pre_process(const std::string& source, ScriptSettings& settings) noexcept;
inline static std::vector<ScriptVariable> parse_argumentlist(std::string source, ScriptSettings& settings) noexcept;
// evaluates an expression and returns the result
inline static  ScriptVariable evaluate_expression(const std::string& source, ScriptSettings& settings) noexcept;
inline static void parse_const_preprog(const std::string& source, ScriptSettings& settings) noexcept;

inline static bool is_operator_char(char) noexcept;

// default lexers, can be accessed and configured by the user 
struct LexerCollection {
    KittenLexer argumentlist = KittenLexer()
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
    KittenLexer expression = KittenLexer()
        .add_stringq('"')
        .add_capsule('(',')')
        .add_capsule('[',']')
        .add_capsule('{','}')
        .add_con_extract(is_operator_char)
        .add_ignore(' ')
        .add_ignore('\t')
        .add_backslashopt('t','\t')
        .add_backslashopt('n','\n')
        .add_backslashopt('r','\r')
        .add_backslashopt('\\','\\')
        .add_backslashopt('"','\"')
        .erase_empty();
    KittenLexer preprocess = KittenLexer()
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
    
    void clear() {
        argumentlist = expression = preprocess = KittenLexer();
    }

    // modify these to customize how source code get turned into tokens

    std::function<lexed_kittens(const std::string&)> p_argumentlist = [&](const std::string& s){ return argumentlist.lex(s); };
    std::function<lexed_kittens(const std::string&)> p_expression = [&](const std::string& s){ return expression.lex(s); };
    std::function<lexed_kittens(const std::string&)> p_preprocess = [&](const std::string& s){ return preprocess.lex(s); };
};

class Interpreter;
// storage class to temporarily store states of the interpreter
struct InterpreterState {
    std::map<std::string,ScriptBuiltin> script_builtins;
    std::map<std::string,std::vector<ScriptOperator>> script_operators;
    std::vector<ScriptTypeCheck> script_typechecks;
    std::unordered_map<std::string,std::string> script_macros;
    std::unordered_map<std::string,ScriptRawBuiltin> script_rawbuiltins;
    std::unordered_map<std::string,ScriptPreProcess> script_preprocesses;

    LexerCollection lexers;

    InterpreterState() {}
    InterpreterState(const Interpreter& interp) { save(interp); }
    InterpreterState(
        const std::map<std::string,ScriptBuiltin>& a,
        const std::map<std::string,std::vector<ScriptOperator>>& b,
        const std::vector<ScriptTypeCheck>& c,
        const std::unordered_map<std::string,std::string>& d,
        const std::unordered_map<std::string,ScriptRawBuiltin>& e,
        std::unordered_map<std::string,ScriptPreProcess> f,
        const LexerCollection& g):
        script_builtins(a),
        script_operators(b),
        script_typechecks(c),
        script_macros(d),
        script_rawbuiltins(e),
        script_preprocesses(f),
        lexers(g) {}

    inline void load(Interpreter& interp) const noexcept;
    inline void save(const Interpreter& interp) noexcept;

    inline InterpreterState& operator=(const std::map<std::string,ScriptBuiltin>& a) noexcept {
        script_builtins = a;
        return *this;
    }
    inline InterpreterState& operator=(const std::map<std::string,std::vector<ScriptOperator>>& a) noexcept {
        script_operators = a;
        return *this;
    }
    inline InterpreterState& operator=(const std::vector<ScriptTypeCheck>& a) noexcept {
        script_typechecks = a;
        return *this;
    }
    inline InterpreterState& operator=(const std::unordered_map<std::string,std::string>& a) noexcept {
        script_macros = a;
        return *this;
    }
    inline InterpreterState& operator=(const std::unordered_map<std::string,ScriptRawBuiltin>& a) noexcept {
        script_rawbuiltins = a;
        return *this;
    }
    inline InterpreterState& operator=(const std::unordered_map<std::string,ScriptPreProcess>& a) noexcept {
        script_preprocesses = a;
        return *this;
    }
    inline InterpreterState& operator=(const LexerCollection& a) noexcept {
        lexers = a;
        return *this;
    }

    inline InterpreterState& add(const std::map<std::string,ScriptBuiltin>& a) noexcept {
        script_builtins.insert(a.begin(),a.end());
        return *this;
    }
    inline InterpreterState& add(const std::map<std::string,std::vector<ScriptOperator>>& a) noexcept {
        for(auto& i : script_operators) {
            for(auto j : a.at(i.first)) {
                i.second.push_back(j);
            }
        }
        return *this;
    }
    inline InterpreterState& add(const std::vector<ScriptTypeCheck>& a) noexcept {
        for(auto i : a) {
            script_typechecks.push_back(i);
        }
        return *this;
    }
    inline InterpreterState& add(const std::unordered_map<std::string,std::string>& a) noexcept {
        script_macros.insert(a.begin(), a.end());
        return *this;
    }
    inline InterpreterState& add(const std::unordered_map<std::string,ScriptRawBuiltin>& a) noexcept {
        script_rawbuiltins.insert(a.begin(), a.end());
        return *this;
    }
    inline InterpreterState& add(const std::unordered_map<std::string,ScriptPreProcess>& a) noexcept {
        script_preprocesses.insert(a.begin(), a.end());
        return *this;
    }
};

// helper class for handling errors
struct InterpreterError  {
    Interpreter& interpreter;
private:
    bool has_value = false;
    ScriptVariable value = script_null;
public:
    InterpreterError(Interpreter& i): interpreter(i) {}
    InterpreterError(Interpreter& i, const ScriptVariable& val): interpreter(i), value(val) { has_value = true; }
    inline InterpreterError& on_error(const std::function<void(Interpreter&)>&) noexcept;
    inline InterpreterError& otherwise(const std::function<void(Interpreter&)>&) noexcept;
    inline InterpreterError& throw_error();
    inline ScriptVariable get_value() const noexcept { return value; }
    inline ScriptVariable get_value_or(const ScriptVariable& var) const noexcept { return has_value ? value : var; }
    inline Interpreter& chain() noexcept { return interpreter; }
    operator ScriptVariable() const noexcept { return value; }
};

struct ExtensionData;

// wrapper and storage class for a simpler API usage
class Interpreter {
    std::vector<ExtensionData> extensions;
    std::map<int,InterpreterState> states;
    std::function<void(Interpreter&)> on_error_f;

    inline void error_check() {
        if(settings.error_msg != "" && on_error_f) on_error_f(*this);
    }
public:
    std::map<std::string,ScriptBuiltin> script_builtins = default_script_builtins;
    std::map<std::string,std::vector<ScriptOperator>> script_operators = default_script_operators;
    std::vector<ScriptTypeCheck> script_typechecks = default_script_typechecks;
    std::unordered_map<std::string,std::string> script_macros = default_script_macros;
    std::unordered_map<std::string,ScriptPreProcess> script_preprocesses;
    std::unordered_map<std::string,ScriptRawBuiltin> script_rawbuiltins;
    
    LexerCollection lexer;
    ScriptSettings settings = ScriptSettings(*this);
    
    inline void save(int id) noexcept {
        states[id].save(*this);
    }

    inline void load(int id) noexcept {
        states[id].load(*this);
    }

    inline void clear() {
        script_builtins.clear();
        script_operators.clear();
        script_typechecks.clear();
        script_macros.clear();
        script_preprocesses.clear();
        script_rawbuiltins.clear();
        lexer.clear();
        settings.clear();
    }

    inline operator bool() const noexcept {
        return settings.error_msg == "";
    }
    inline operator ScriptSettings&() noexcept {
        return settings;
    }

    inline InterpreterError pre_process(std::string source) noexcept {
        settings.error_msg = "";
        settings.labels = ::carescript::pre_process(source,settings);
        error_check();
        return *this;
    }

    inline InterpreterError run() noexcept {
        settings.return_value = script_null;
        settings.line = 1;
        settings.exit = false;
        settings.error_msg = run_label("main", settings.labels, settings, "", {});
        settings.exit = false;
        error_check();
        return is_null(settings.return_value) ? *this : InterpreterError(*this, settings.return_value);
    }

    template<typename... Targs>
    inline InterpreterError run(std::string label, Targs ...targs) {
        std::vector<ScriptVariable> args = {targs...};
        return run(label, args);
    }
    inline InterpreterError run(std::string label, std::vector<ScriptVariable> args) noexcept {
        settings.return_value = script_null;
        settings.line = 1;
        settings.exit = false;
        settings.error_msg = run_label(label, settings.labels, settings, "", args);
        settings.exit = false;
        error_check();
        return is_null(settings.return_value) ? *this : InterpreterError(*this, settings.return_value);
    }

    inline InterpreterError eval(std::string source) noexcept {
        settings.return_value = script_null;
        settings.error_msg = run_script(source,settings);
        settings.exit = false;
        error_check();
        return is_null(settings.return_value) ? *this : InterpreterError(*this,settings.return_value);
    }
    inline InterpreterError expression(std::string source) {
        auto ret = evaluate_expression(source,settings);
        error_check();
        return is_null(ret) ? *this : InterpreterError(*this,ret);
    }

    inline int to_local_line(const int& line) const noexcept { return line - settings.labels.at(settings.label.top()).line; }
    inline int to_global_line(const int& line) const noexcept { return line + settings.labels.at(settings.label.top()).line; }

    inline void on_error(const std::function<void(Interpreter&)>& fun) noexcept {
        on_error_f = fun;
    }

    inline std::string error() const noexcept { return settings.error_msg; }

    inline Interpreter& add_builtin(const std::string& name, const ScriptBuiltin& builtin) noexcept {
        script_builtins[name] = builtin;
        return *this;
    }
    inline Interpreter& add_operator(const std::string& name, const ScriptOperator& _operator) {
        if(lexer.p_expression(name).size() != 1) 
            throw "Carescript: Operator name must be 1 token";
        script_operators[name].push_back(_operator);
        return *this;
    }
    inline Interpreter& add_typecheck(const ScriptTypeCheck& typecheck) noexcept {
        script_typechecks.push_back(typecheck);
        return *this;
    }
    inline Interpreter& add_macro(const std::string& macro, const std::string& replacement) noexcept {
        script_macros[macro] = replacement;
        return *this;
    }
    inline Interpreter& add_rawbuiltin(const std::string& name, const ScriptRawBuiltin& rawbuiltin) noexcept {
        script_rawbuiltins[name] = rawbuiltin;
        return *this;
    }
    inline Interpreter& add_preprocess(const std::string& name, const ScriptPreProcess& process) noexcept {
        script_preprocesses[name] = process;
        return *this;
    }

    inline InterpreterError bake(std::string file) noexcept {
        if(!bake_extension(file, settings)) 
            settings.error_msg = "error while baking: " + file;
        return *this;
    }
    inline InterpreterError bake(ExtensionData ext) noexcept;

    inline bool has_builtin(const std::string& name) const noexcept {
        return script_builtins.find(name) != script_builtins.end();
    }
    inline ScriptBuiltin& get_builtin(const std::string& name) {
        return script_builtins[name];
    }
    inline bool has_macro(const std::string& name) const noexcept {
        return script_macros.find(name) != script_macros.end();
    }
    std::string& get_macro(const std::string& name) noexcept {
        return script_macros[name];
    }
    inline bool has_operator(const std::string& name) const noexcept {
        return script_operators.find(name) != script_operators.end();
    }
    inline std::vector<ScriptOperator>& get_operator(const std::string& name) noexcept {
        return script_operators[name];
    }
    inline bool has_rawbuiltin(const std::string& name) const noexcept {
        return script_rawbuiltins.find(name) != script_rawbuiltins.end();
    }
    inline ScriptRawBuiltin& get_rawbuiltin(const std::string& name) noexcept {
        return script_rawbuiltins[name];
    }
    inline bool has_preprocess(const std::string& name) const noexcept {
        return script_preprocesses.find(name) != script_preprocesses.end();
    }
    inline ScriptPreProcess& get_preprocess(const std::string& name) noexcept {
        return script_preprocesses[name];
    }

    inline bool has_variable(const std::string& name) const noexcept {
        return settings.variables.find(name) != settings.variables.end();
    }
    inline ScriptVariable& get_variable(const std::string& name) noexcept {
        return settings.variables[name];
    }

    inline Extension* get_extension(const std::string& id) const noexcept;
    inline Extension* get_extension(const size_t& type_id) const noexcept;

    template<typename Cast>
    inline Cast& get_extension(const std::string& id) const {
        return *(Cast*)get_extension(id);
    }

    template<typename TExtension>
    inline TExtension* get_extension() const {
        return dynamic_cast<TExtension*>(get_extension(typeid(TExtension).hash_code()));
    }

    inline bool has_extension(const std::string& id) const noexcept {
        return get_extension(id) != nullptr;
    }
    
    friend inline bool bake_extension(ExtensionData ext, ScriptSettings& settings) noexcept;

    ~Interpreter();
};

inline InterpreterError& InterpreterError::on_error(const std::function<void(Interpreter&)>& fun) noexcept {
    if(!interpreter) fun(interpreter);
    return *this;
}

inline InterpreterError& InterpreterError::otherwise(const std::function<void(Interpreter&)>& fun) noexcept {
    if(interpreter) fun(interpreter);
    return *this;
}

inline InterpreterError& InterpreterError::throw_error() {
    if(!interpreter) throw interpreter;
    return *this;
}

inline void InterpreterState::load(Interpreter& interp) const noexcept {
    interp.clear();
    interp.script_builtins = this->script_builtins;
    interp.script_operators = this->script_operators;
    interp.script_typechecks = this->script_typechecks;
    interp.script_macros = this->script_macros;
    interp.script_preprocesses = this->script_preprocesses;
    interp.script_rawbuiltins = this->script_rawbuiltins;
    interp.lexer = this->lexers;
}
inline void InterpreterState::save(const Interpreter& interp) noexcept {
    script_builtins = interp.script_builtins;
    script_operators = interp.script_operators;
    script_typechecks = interp.script_typechecks;
    script_macros = interp.script_macros;
    script_preprocesses = interp.script_preprocesses;
    script_rawbuiltins = interp.script_rawbuiltins;
    lexers = interp.lexer;
}

// converts a literal into a variable
inline static ScriptVariable to_var(const KittenToken& src, ScriptSettings& settings) noexcept {
    ScriptVariable ret;
    for(auto i : settings.interpreter.script_typechecks) {
        ScriptValue* v = i(src, settings);
        if(v != nullptr) {
            return std::move(ScriptVariable(v));
        }
    }
    return script_null;
}

// checks if the token is a valid literal
inline static bool valid_literal(const KittenToken& src, ScriptSettings& settings) noexcept {
    for(auto i : settings.interpreter.script_typechecks) {
        ScriptValue* v = i(src, settings);
        if(v != nullptr) {
            delete v;
            return true;
        }
    }
    return false;
}

// sets the error flag is the variable is null
inline static ScriptVariable not_null_check(const ScriptVariable& var, ScriptSettings& settings) noexcept {
    if(is_null(var)) {
        settings.error_msg = "not allowed to be null!";
    }
    return var;
}

#define CARESCRIPT_EXTENSION using namespace carescript;
#define CARESCRIPT_EXTENSION_GETEXT(...) \
extern "C" { \
    carescript::ExtensionData get_extension() { \
        using t = std::remove_pointer<decltype([&](){ __VA_ARGS__ }())>::type; \
        return carescript::ExtensionData::make<t>( \
            [&]()->t*{ __VA_ARGS__ }() \
        ); \
    } \
}
#define CARESCRIPT_EXTENSION_GETEXT_INLINE(...) \
extern "C" { \
    inline static carescript::ExtensionData get_extension() { \
        using t = std::remove_pointer<decltype([&](){ __VA_ARGS__ }())>::type; \
        return carescript::ExtensionData::make<t>( \
            [&]()->t*{ __VA_ARGS__ }() \
        ); \
    } \
}

using BuiltinList = std::unordered_map<std::string,ScriptBuiltin>;
using OperatorList = std::unordered_map<std::string,std::vector<ScriptOperator>>;
using MacroList = std::unordered_map<std::string,std::string>;
using TypeList = std::vector<ScriptTypeCheck>;
using RawBuiltinList = std::unordered_map<std::string,ScriptRawBuiltin>;
using PreProcList = std::unordered_map<std::string,ScriptPreProcess>;

// abstract class to provide an interface for extensions
class Extension {
    Interpreter* interp_link = nullptr;
public:
    virtual const char* id() const noexcept { return "anon"; }
    
    virtual BuiltinList get_builtins() { return {}; }
    virtual OperatorList get_operators() { return {}; }
    virtual MacroList get_macros() { return {}; }
    virtual TypeList get_types() { return {}; }
    virtual RawBuiltinList get_rawbuiltins() { return {}; }
    virtual PreProcList get_preprocesses() { return {}; }

    virtual void poke_interpreter(Interpreter&) { return; }

    inline Interpreter& get_interpreter() noexcept { return *interp_link; }
    inline bool orphan() const noexcept { return interp_link == nullptr; }
    inline Extension& self() noexcept { return *this; }

    friend inline bool bake_extension(ExtensionData ext, ScriptSettings& settings) noexcept;
};

struct ExtensionData {
    size_t hash = 0;
    Extension* extension = nullptr;

    ExtensionData() {}

    template<typename T> requires std::is_base_of_v<Extension,T>
    ExtensionData(T* ext) {
        hash = typeid(T).hash_code();
        extension = ext;
    }

    ExtensionData(size_t h, Extension* ext): hash(h), extension(ext) {} 

    template<typename T> requires std::is_base_of_v<Extension,T>
    static inline ExtensionData make(T* ext) noexcept {
        return ExtensionData(typeid(T).hash_code(),ext);
    }
};

inline Extension* Interpreter::get_extension(const std::string& id) const noexcept {
    for(auto& i : extensions) 
        if(id == i.extension->id()) return i.extension;
    return nullptr;
}

inline Extension* Interpreter::get_extension(const size_t& type_id) const noexcept {
    for(auto& i : extensions) 
        if(type_id == i.hash) return i.extension;
    return nullptr;
}

inline Interpreter::~Interpreter() {
    for(auto& i : extensions) {
        delete i.extension; // TODO: frees not enough bytes
        i.extension = nullptr;
    }
}

inline InterpreterError Interpreter::bake(ExtensionData ext) noexcept {
    if(!bake_extension(ext,settings)) 
        settings.error_msg = "error while baking: <compiled>";
    return *this;
}

using get_extension_fun = ExtensionData(*)();

// external overloads for the ScriptVariable constructor

template<typename Tp> requires std::is_arithmetic_v<Tp>
inline static void from(carescript::ScriptVariable& var, const Tp& integral) noexcept {
    var = new carescript::ScriptNumberValue(integral);
}

inline static void from(carescript::ScriptVariable& var, const std::string& string) noexcept{
    var = new carescript::ScriptStringValue(string);
}

} /* namespace carescript */

#endif