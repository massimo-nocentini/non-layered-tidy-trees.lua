
/* 
	This is a glue c file for importing delta client c functions into Lua workflow.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <time.h>
#include <non-layered-tidy-trees.h>

typedef struct userdata_s {
    lua_State *L;
    lua_Integer table_absidx;
} userdata_t;

static int l_mktree(lua_State *L) {

	lua_Integer id = lua_tointeger (L, -4);
	lua_Number w = lua_tonumber(L, -3);
	lua_Number h = lua_tonumber(L, -2);
	lua_Integer cs = lua_tointeger(L, -1);

	tree_t *t = (tree_t *) malloc (sizeof(tree_t));

	t->id = id;
	t->w = w;
	t->h = h;
	t->cs = cs;
	t->c = cs == 0 ? NULL : (tree_t **) malloc (sizeof(tree_t *) * cs);

	lua_pushlightuserdata (L, t);

	return 1;

}

static int l_atputchild(lua_State *L) {

	tree_t *parent = (tree_t *) lua_touserdata (L, -3);
	lua_Integer i = lua_tointeger(L, -2);
	tree_t *child = (tree_t *) lua_touserdata (L, -1);
	
	parent->c[i] = child;

	return 0;
}

static int l_dbind(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata (L, -1);
	
	lua_pushnumber (L, t->x);
	lua_pushnumber (L, t->y);
	lua_pushnumber (L, t->prelim);
	lua_pushinteger (L, t->id);

	return 4;
}

static void treefree (tree_t *t) {

	for (int i = 0; i < t->cs; i++) treefree (t->c[i]);

	free (t->c);
	free (t);

}

static int l_free(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata (L, -1);
	
	treefree (t);

	return 0;
}


static int l_updatewh(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata (L, -3);
	lua_Number w = lua_tonumber(L, -2);
	lua_Number h = lua_tonumber(L, -1);
	
	t->w = w;
	t->h = h;

	return 0;
}

static int l_layout(lua_State *L) {

	tree_t *root = (tree_t *) lua_touserdata(L, -1);

	layout (root, NULL, NULL, NULL);

	return 0;
}

static const struct luaL_Reg tidytree_reg [] = {
	{"layout", l_layout},
	{"mktree", l_mktree},
	{"free", l_free},
	{"updatewh", l_updatewh},
	{"atputchild", l_atputchild},
	{"dbind", l_dbind},
	{NULL, NULL} /* sentinel */
};
 
int luaopen_libluanonlayeredtidytrees (lua_State *L) {
	luaL_newlib(L, tidytree_reg);
	return 1;
}
