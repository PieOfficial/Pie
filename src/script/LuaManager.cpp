#include "LuaManager.h"

LuaManager* LuaManager::instance_ = nullptr;

LuaManager::LuaManager() {
    L_ = luaL_newstate();
    luaL_openlibs(L_);
}

LuaManager::~LuaManager() {
    lua_close(L_);
}

LuaManager& LuaManager::getInstance() {
    if (!instance_) {
        instance_ = new LuaManager();
    }
    return *instance_;
}

lua_State* LuaManager::getState() const {
    return L_;
}
