#ifndef LUAMANAGER_H
#define LUAMANAGER_H
#include <lua.hpp>

#include <memory>
#include <mutex>

class LuaManager {
public:
    static LuaManager& getInstance();
    lua_State* getState() const;

    LuaManager();
    ~LuaManager();

    lua_State* L_;
    static std::unique_ptr<LuaManager> instance_;
    static std::mutex instance_mutex_;
};

#endif // LUAMANAGER_H