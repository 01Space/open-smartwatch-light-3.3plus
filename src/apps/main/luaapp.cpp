
#include "./apps/main/luaapp.h"

#include <osw_app.h>
#include <osw_hal.h>

void OswLuaApp::setup(OswHal* hal) {
    luaState = luaL_newstate();

    if (luaState) {
        luaL_openlibs(luaState);

        halToLua(luaState, hal);

        if (luaL_dostring(luaState, fileStr)) {
            Serial.println("Failed to run file");
            cleanupState();
            return;
        }

        lua_getglobal(luaState, LUA_SETUP_FUNC);
        if (lua_pcall(luaState, 0, 0, 0)) {
            Serial.println("Failed to call setup");
        }
    }
}

void OswLuaApp::loop(OswHal* hal) {
    if (luaState) {
        lua_getglobal(luaState, LUA_LOOP_FUNC);
        if (lua_pcall(luaState, 0, 0, 0)) {
            Serial.println("Failed to call loop"); 
        }

        //Force GC to run after loop
        lua_gc(luaState, LUA_GCCOLLECT, 0);
    }
}

void OswLuaApp::stop(OswHal* hal) {
    if (luaState) {
        lua_getglobal(luaState, LUA_STOP_FUNC);
        if (lua_pcall(luaState, 0, 0, 0)) {
            Serial.println("Failed to call stop");
        }
    }

    cleanupState();
}

void OswLuaApp::cleanupState() {
    if (luaState) {
        lua_close(luaState);
        luaState = NULL;
    }
}