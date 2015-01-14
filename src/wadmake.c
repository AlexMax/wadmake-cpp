#include <stdio.h>

#include <bstrlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef enum {
	OK,
	ERROR
} error;

int main(int argc, char* argv) {
	const char* filename = "wadmakefile";

	lua_State* L = luaL_newstate();

	if (luaL_dofile(L, filename) == 1) {
		printf("wadmake: error while parsing %s\n");
		goto cleanup;
	}

	lua_close(L);
	return 0;

cleanup:
	lua_close(L);
	return 1;
}
