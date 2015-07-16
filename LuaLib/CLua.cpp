#include <windows.h>
#include <CLua.h>
#include <string>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luaconf.h"
}

cLua::cLua() {
    m_pErrorHandler = NULL;
    m_pScriptContext = luaL_newstate();
    luaL_openlibs(m_pScriptContext);
}

cLua::~cLua() {
    if (m_pScriptContext) {
        lua_close(m_pScriptContext);
    }
}

static std::string findScript(const char* pFname) {
    FILE* fTest;

    char drive[_MAX_DRIVE + 1];
    char dir[_MAX_DIR + 1];
    char fname[_MAX_FNAME + 1];
    char ext[_MAX_EXT + 1];

    _splitpath_s(pFname, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

    std::string strTestFile = (std::string) drive + dir + "Scripts\\" + fname + ".LUB";
    int nRet = fopen_s(&fTest, strTestFile.c_str(), "r"); // ³É¹¦·µ»Ø0
    if (nRet != 0) {
        //not that one...
        strTestFile = (std::string) drive + dir + "Scripts\\" + fname + ".LUA";
        nRet = fopen_s(&fTest, strTestFile.c_str(), "r");
    }

    if (nRet != 0) {
        //not that one...
        strTestFile = (std::string) drive + dir + fname + ".LUB";
        nRet = fopen_s(&fTest, strTestFile.c_str(), "r");
    }

    if (nRet != 0) {
        //not that one...
        //not that one...
        strTestFile = (std::string) drive + dir + fname + ".LUA";
        nRet = fopen_s(&fTest, strTestFile.c_str(), "r");
    }

    if (nRet == 0) {
        fclose(fTest);
    }

    return strTestFile;
}

bool cLua::RunScript(const char* pFname) {
    std::string strFilename = findScript(pFname);
    const char* pFilename = strFilename.c_str();

    if (0 != luaL_dofile(m_pScriptContext, pFilename)) {
        if (m_pErrorHandler) {
            char buf[256];
            sprintf_s(buf, 255, "Lua Error - Script Load\r\nScript Name:%s\r\nError Message:%s\r\n", pFilename, luaL_checkstring(m_pScriptContext, -1));
            m_pErrorHandler(buf);
        }
        return false;
    }

    return true;

}

bool cLua::RunString(const char* pCommand) {
    if (0 != luaL_dostring(m_pScriptContext, pCommand)) {
        if (m_pErrorHandler) {
            char buf[256];
            sprintf_s(buf, 255, "Lua Error - String Load\r\nString:%s\r\nError Message:%s\r\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
            m_pErrorHandler(buf);
        }
        return false;
    }

    return true;
}

const char* cLua::GetErrorString(void) {
    return luaL_checkstring(m_pScriptContext, -1);
}


void cLua::AddFunction(const char* pFunctionName, LuaFunctionType pFunction) {
    lua_register(m_pScriptContext, pFunctionName, pFunction);
}

const char* cLua::GetStringArgument(int num, const char* pDefault /*= nullptr*/) {
    return luaL_optstring(m_pScriptContext, num, pDefault);
}

double cLua::GetNumberArgument(int num, double dDefault /*= 0.0*/) {
    return luaL_optnumber(m_pScriptContext, num, dDefault);
}

int cLua::GetIntArgument(int num, int nDefault /*= 0*/) {
    return luaL_optinteger(m_pScriptContext, num, nDefault);
}

void cLua::PushString(const char* pString) {
    lua_pushstring(m_pScriptContext, pString);
}

void cLua::PushNumber(double value) {
    lua_pushnumber(m_pScriptContext, value);
}

void cLua::PushInt(int value) {
    lua_pushinteger(m_pScriptContext, value);
}

void cLua::SetErrorHandler(void(*pErrHandler)(const char* pError)) {
    m_pErrorHandler = pErrHandler;
}

lua_State* cLua::GetScriptContext(void) {
    return m_pScriptContext;
}

void cLua::GarbageCollection() {
    lua_gc(m_pScriptContext, LUA_GCCOLLECT, 0);
}
