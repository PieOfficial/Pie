#include <lua.hpp>

class LuaManager {
public:
    static LuaManager& getInstance();
    lua_State* getState() const;

private:
    LuaManager();
    ~LuaManager();
    LuaManager(const LuaManager&) = delete;
    LuaManager& operator=(const LuaManager&) = delete;

    lua_State* L_;
    static LuaManager* instance_;
};
