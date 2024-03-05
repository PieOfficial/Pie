#include "LuaManager.h"
#include <memory>
#include <mutex>

std::unique_ptr<LuaManager> LuaManager::instance_;
std::mutex LuaManager::instance_mutex_;

LuaManager::LuaManager() {
    L_ = luaL_newstate();
    luaL_openlibs(L_);
}

LuaManager::~LuaManager() {
    lua_close(L_);
}

LuaManager& LuaManager::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::make_unique<LuaManager>();
    }
    return *instance_;
}

lua_State* LuaManager::getState() const {
    return L_;
}