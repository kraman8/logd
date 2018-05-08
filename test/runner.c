#include <getopt.h>
#include <stdio.h>

#include "../src/lua/lauxlib.h"
#include "../src/lua/lua.h"
#include "../src/lua/lualib.h"

#include "../src/lua.h"

#define LUA_NAME_IT "it"
#define LUA_NAME_TEST_MODULE "test"

struct args_s {
	int help;
} args;

struct test_s {
	struct test_s* next;
	char* desc;
} tests;

int lua_test_it(lua_State* state)
{
	const char* desc = luaL_checkstring(state, 1);
	if (desc == NULL) {
		return luaL_error(state, "expected 1st argument of '%s' to be a string", LUA_NAME_IT);
	}

	if (!lua_isfunction(state, 2)) {
		return luaL_error(state, "expected 2nd argument of '%s' to be a function or closure with the test to run", LUA_NAME_IT);
	}

	return lua_pcall(state, 1, 0, 0);
}

static const struct luaL_Reg test_functions[] = {
  {LUA_NAME_IT, &lua_test_it}, {NULL, NULL}};

void lua_load_testing_utils(lua_t* l)
{
	luaL_register(l->state, LUA_NAME_TEST_MODULE, test_functions);
}

void args_init(int argc, char* argv[], char** script)
{
	/* set defaults for arguments */
	args.help = 0;

	static struct option long_options[] = {
	  {"help", no_argument, 0, 'h'}, {0, 0, 0, 0}};

	int option_index = 0;
	int c = 0;
	while (
	  (c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
		switch (c) {
		case 'h':
			args.help = 1;
			break;
		default:
			abort();
		}
	}

	*script = argv[optind];
}

void print_usage(const char* exe)
{
	printf("usage: %s <script> [options]\n", exe);
	printf("\noptions:\n");
	printf("\t-h, --help          prints this message\n");
}

int main(int argc, char* argv[])
{
	int ret = 0;
	char* script;
	lua_t* l;

	args_init(argc, argv, &script);

	if ((l = lua_create(script)) == NULL) {
		perror("lua_create");
		ret = 1;
		goto exit;
	}

	lua_load_testing_utils(l);

exit:
	lua_free(l);
	return ret;
}
