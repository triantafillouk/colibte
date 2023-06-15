/* macro language lua connector */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "xe.h"
static double result=0;

static void stackDump (lua_State *L,char *title) {
  int i;
  int top = lua_gettop(L);
  MESG("lua stack Dump:%s top=%d",title,top);
  for (i = 0; i <= top; i++) {  /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {

      case LUA_TSTRING:  /* strings */
        MESG("%d: string: `%s'",i, lua_tostring(L, i));
        break;

      case LUA_TBOOLEAN:  /* booleans */
        MESG("%d: boolean: %s",i,lua_toboolean(L, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:  /* numbers */
        MESG("%d: number: %g",i, lua_tonumber(L, i));
        break;

      default:  /* other values */
        MESG("%d: other %s",i, lua_typename(L, t));
        break;
    }
  }
  MESG("---------------------------------");  /* end the listing */
}

int multiplication(lua_State *L) {

    // Check if the first argument is integer and return the value
    int a = luaL_checknumber(L, 1);

    // Check if the second argument is integer and return the value
    int b = luaL_checknumber(L, 2);
	double c1=a*b+1.33;
    // multiply and store the result inside a type lua_Integer
    lua_Number c = c1;
	// double f1=11.33;
    // Here we prepare the values to be returned.
    // First we push the values we want to return onto the stack in direct order.
    // Second, we must return the number of values pushed onto the stack.

    // Pushing the result onto the stack to be returned
    lua_pushnumber(L, c);
	stackDump(L,"before ending function");
	
	result = c1;
    return 1; // The number of returned values
}

int lua_vsplit_window(lua_State *L) {
	vsplit_window(1);
	return 0;
}

int exec_lua(int n)
{
 lua_State *L = luaL_newstate();
 luaL_openlibs(L);
 double r1=0;
    // Push the pointer to function
    lua_pushcfunction(L, multiplication);

    // Get the value on top of the stack
    // and set as a global, in this case is the function
    lua_setglobal(L, "mul");

	lua_pushcfunction(L, lua_vsplit_window);
	lua_setglobal(L,"vsplit_window");
 char *code = "lua_result = mul(7,8);vsplit_window()";

    if (luaL_dostring(L, code) == LUA_OK) {
		lua_getglobal (L, "lua_result");
        lua_pop(L, lua_gettop(L));
    };
	r1 = lua_tonumber(L,0);

	stackDump(L,"after doing string");
	MESG("r1=%f",r1);
	// stackDump(L,"after poping top");
	
    lua_close(L);

 return (int)result;
}

