#include "carescript-api.hpp"

CARESCRIPT_EXTENSION

// Implent a new type by adding a class that implements the ScriptValue interface
class ListType : public ScriptValue {
public:
    using val_t = std::vector<ScriptVariable>;
    val_t list;

    val_t get_value() const noexcept { return list; }
    val_t& get_value() noexcept { return list; }
    const std::string get_type() const noexcept override { return "List"; }
    bool operator==(const ScriptValue* p) const noexcept override {
        return p->get_type() == get_type() && ((ListType*)p)->get_value() == get_value();
    };
    std::string to_printable() const noexcept override {
        std::string s = "[";
        for(auto i : list) {
            s += i.string() + ",";
        }
        if(s != "[") s.pop_back();
        return s + "]";
    }
    std::string to_string() const noexcept override {
        return to_printable();
    }
    ScriptValue* copy() const noexcept override {
        return new ListType(list);
    }

    ListType() {}
    ListType(val_t v) : list(v) {}
};

// write a main extension class that implements the Extension interface
class ListExtention : public Extension {
public:
    // functions to add
    BuiltinList get_builtins() override {
        return {
            // usage: push(list,to_push1,to_push2,...)
            {"push",{-1,[](const ScriptArglist& args,ScriptSettings& settings)->ScriptVariable {
                cc_builtin_if_ignore();
                // Note how we can already use "ListType" here as any other type
                cc_builtin_var_requires(args[0],ListType);
                cc_builtin_arg_min(args,2);
                ListType list = get_value<ListType>(args[0]);
                for(size_t i = 1; i < args.size(); ++i) {
                    list.list.push_back(args[i]);
                }
                return new ListType(list);
            }}},
            // usage: pop(list,[times])
            {"pop",{-1,[](const ScriptArglist& args,ScriptSettings& settings)->ScriptVariable {
                cc_builtin_if_ignore();
                cc_builtin_var_requires(args[0],ListType);
                cc_builtin_arg_range(args,1,2);
                size_t count = 1;
                if(args.size() == 2) {
                    cc_builtin_var_requires(args[1],ScriptNumberValue);
                    count = (size_t)get_value<ScriptNumberValue>(args[1]);
                }
                ListType list = get_value<ListType>(args[0]);
                _cc_error_if(list.list.size() <= count,"popped not existing element (size below 0)");
                for(size_t i = 0; i < count; ++i) list.list.pop_back();
                return new ListType(list);
            }}}
        };
    }

    // to add some operators
    OperatorList get_operators() { 
        return {
            // adds a "+" operator to join lists together
            {{"+",{{0,ScriptOperator::BINARY,[](const ScriptVariable& left, const ScriptVariable& right, ScriptSettings& settings)->ScriptVariable {
                cc_operator_same_type(right,left,"+");
                cc_operator_var_requires(right,"+",ListType);
                auto lvec = get_value<ListType>(left);
                auto rvec = get_value<ListType>(right);
                for(auto& i : rvec) lvec.push_back(i);
                return new ListType(lvec);
            }}}}},
        }; 
    }

    // to implement what a list literal looks like
    // example: [1,2, 4*5, "hello"]
    TypeList get_types() { 
        return {
            [](KittenToken src, ScriptSettings& settings)->ScriptValue* {
                if(src.str) return nullptr; // on error, return a nullptr
                KittenLexer lx = KittenLexer()
                    .add_capsule('[',']')
                    .add_ignore(',')
                    .erase_empty()
                    .add_stringq('"')
                    ;
                auto lex1 = lx.lex(src.src);
                if(lex1.size() != 1) return nullptr;
                lex1[0].src.erase(lex1[0].src.begin());
                lex1[0].src.erase(lex1[0].src.end()-1);

                auto plist = lx.lex(lex1[0].src);
                ListType list;
                for(auto i : plist) {
                    if(i.str) i.src = "\"" + i.src + "\"";
                    list.list.push_back(evaluate_expression(i.src,settings));
                }
                return new ListType(list.list);
            }
        }; 
    }
};

// At the end, define how the extension gets instances
CARESCRIPT_EXTENSION_GETEXT(
    return new ListExtention(); // this will get freed automatically, don't worry
)