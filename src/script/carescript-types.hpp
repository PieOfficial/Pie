#ifndef CARESCRIPT_TYPES_HPP
#define CARESCRIPT_TYPES_HPP

#include <string>
#include <vector>

namespace carescript {

// abstract class to provide an interface for all types
struct ScriptValue {
    using type = void;
    virtual const std::string get_type() const noexcept= 0;
    virtual bool operator==(const ScriptValue*) const noexcept = 0;
    virtual bool operator==(const ScriptValue&v) const noexcept { return operator==(&v); }
    virtual std::string to_printable() const noexcept = 0;
    virtual std::string to_string() const noexcept = 0;
    virtual ScriptValue* copy() const noexcept = 0;
    void get_value() const noexcept {}
    void get_value() noexcept {}

    virtual ~ScriptValue() {};
};



// default number type implementation
struct ScriptNumberValue : public ScriptValue {
    const std::string get_type() const noexcept override { return "Number"; }
    long double number = 0.0;

    bool operator==(const ScriptValue* val) const noexcept override {
        return val->get_type() == get_type() && ((ScriptNumberValue*)val)->number == number;
    }

    /**
     * Converts the number to a printable string representation.
     *
     * @return the printable string representation of the number
     *
     * @throws none
     */
    std::string to_printable() const noexcept override {
        std::string str = std::to_string(number);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
        return str;
    }
    std::string to_string() const noexcept override {
        return to_printable();
    }

    long double get_value() const noexcept { return number; }
    long double& get_value() noexcept { return number; }
    ScriptValue* copy() const noexcept override { return new ScriptNumberValue(number); }

    ScriptNumberValue() {}
    ScriptNumberValue(long double num): number(num) {}

    operator long double() const noexcept { return get_value(); }
};

// default string type implementation
struct ScriptStringValue : public ScriptValue {
    const std::string get_type() const noexcept override { return "String"; }
    std::string string;
    
    /**
     * Checks if the given ScriptValue object is equal to this ScriptValue object.
     *
     * @param val The ScriptValue object to compare with.
     *
     * @return true if the given ScriptValue object is equal to this ScriptValue object,
     *         false otherwise.
     *
     * @throws None
     */
    bool operator==(const ScriptValue* val) const noexcept override {
        return val->get_type() == get_type() && ((ScriptStringValue*)val)->string == string;
    }

    std::string to_printable() const noexcept override {
        return string;
    }
    std::string to_string() const noexcept override {
        return "\"" + string + "\"";
    }

    std::string get_value() const noexcept { return string; }
    std::string& get_value() noexcept { return string; }
    ScriptValue* copy() const noexcept override { return new ScriptStringValue(string); }

    ScriptStringValue() {}
    ScriptStringValue(const std::string& str): string(str) {}

    operator std::string() const noexcept { return get_value(); }
};

// default name type implementation
struct ScriptNameValue : public ScriptValue {
    const std::string get_type() const noexcept override { return "Name"; }
    std::string name;
    
    bool operator==(const ScriptValue* val) const noexcept override {
        return val->get_type() == get_type() && ((ScriptNameValue*)val)->name == name;
    }

    std::string to_printable() const noexcept override {
        return name;
    }
    std::string to_string() const noexcept override {
        return to_printable();
    }

    std::string get_value() const noexcept { return name; }
    std::string& get_value() noexcept { return name; }
    ScriptValue* copy() const noexcept override { return new ScriptNameValue(name); }

    ScriptNameValue() {}
    ScriptNameValue(std::string name): name(name) {}
};

// default null type implementation
struct ScriptNullValue : public ScriptValue {
    const std::string get_type() const noexcept override { return "Null"; }
    
    /**
     * Check if the given ScriptValue pointer is equal to this ScriptValue.
     *
     * @param val the ScriptValue pointer to compare with
     *
     * @return true if the given pointer has the same type as this ScriptValue, false otherwise
     *
     * @throws none
     */
    bool operator==(const ScriptValue* val) const noexcept override {
        return val->get_type() == get_type();
    }

    /**
     * Converts the object to a printable string representation.
     *
     * @return the string "null"
     */
    std::string to_printable() const noexcept override {
        return "null";
    }
    std::string to_string() const noexcept override {
        return to_printable();
    }

    void get_value() const noexcept { return; }
    ScriptValue* copy() const noexcept override { return new ScriptNullValue(); }

    ScriptNullValue() {}
};

} /* namespace carescript */

#endif