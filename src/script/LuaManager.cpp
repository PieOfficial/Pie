#include "LuaManager.h"

LuaManager* LuaManager::instance_ = nullptr;

/**
 * Constructor for the LuaManager class.
 *
 * @return void
 *
 * @throws None
 */
LuaManager::LuaManager() {
    L_ = luaL_newstate();
    luaL_openlibs(L_);
}

/**
 * Destructor for the LuaManager class.
 *
 * @throws None
 */
LuaManager::~LuaManager() {
    lua_close(L_);
}

LuaManager& LuaManager::getInstance() {
    if (!instance_) {
        instance_ = new LuaManager();
    }
    return *instance_;
}

/**
 * Returns the Lua state.
 *
 * @return The Lua state.
 */
lua_State* LuaManager::getState() const {
    return L_;
}
