#pragma once

#ifdef LUA_SCRIPTS
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
};

void halToLua(lua_State *L, OswHal *hal);
#endif