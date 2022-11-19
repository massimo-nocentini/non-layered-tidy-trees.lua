
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

	lua_Integer id = lua_tointeger (L, -6);
	lua_Number w = lua_tonumber(L, -5);
	lua_Number h = lua_tonumber(L, -4);
	lua_Integer cs = lua_tointeger(L, -3);
	lua_Number x = lua_tonumber(L, -2);
	lua_Number y = lua_tonumber(L, -1);
	
	tree_t *t = (tree_t *) malloc (sizeof(tree_t));

	t->id = id;
	t->w = w;
	t->h = h;
	t->x = x;
	t->y = y;
	t->prelim = 0.0;
	t->mod = 0.0;
	t->shift = 0.0;
	t->change = 0.0;
	t->msel = 0.0;
	t->mser = 0.0;
	t->tl = NULL;
	t->tr = NULL;
	t->el = NULL;
	t->er = NULL;
	t->cs = cs;
	t->c = cs == 0 ? NULL : (tree_t **) malloc (sizeof(tree_t *) * cs);
	t->p = NULL;

	lua_pushlightuserdata (L, t);

	return 1;

}

static int l_atputchild(lua_State *L) {

	tree_t *parent = (tree_t *) lua_touserdata (L, -3);
	lua_Integer i = lua_tointeger(L, -2);
	tree_t *child = (tree_t *) lua_touserdata (L, -1);
	
	parent->c[i - 1] = child;
	child->p = parent;

	return 0;
}

static int l_dbind(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata (L, -1);

	lua_newtable (L);
	
	lua_pushinteger (L, t->id);
	lua_setfield (L, -2, "id");

	lua_pushnumber (L, t->w);
	lua_setfield (L, -2, "w");

	lua_pushnumber (L, t->h);
	lua_setfield (L, -2, "h");

	lua_pushnumber (L, t->x);
	lua_setfield (L, -2, "x");

	lua_pushnumber (L, t->y);
	lua_setfield (L, -2, "y");

	lua_pushnumber (L, t->prelim);
	lua_setfield (L, -2, "prelim");

	lua_pushnumber (L, t->mod);
	lua_setfield (L, -2, "mod");

	lua_pushnumber (L, t->shift);
	lua_setfield (L, -2, "shift");

	lua_pushnumber (L, t->change);
	lua_setfield (L, -2, "change");
	
	lua_pushnumber (L, t->msel);
	lua_setfield (L, -2, "msel");

	lua_pushnumber (L, t->mser);
	lua_setfield (L, -2, "mser");

	lua_pushinteger (L, t->cs);
	lua_setfield (L, -2, "cs");

	if (t->tl != NULL) {
		lua_pushinteger (L, t->tl->id);
		lua_setfield (L, -2, "tl");
	}

	if (t->tr != NULL) {
		lua_pushinteger (L, t->tr->id);
		lua_setfield (L, -2, "tr");
	}

	if (t->el != NULL) {
		lua_pushinteger (L, t->el->id);
		lua_setfield (L, -2, "el");
	}

	if (t->er != NULL) {
		lua_pushinteger (L, t->er->id);
		lua_setfield (L, -2, "er");
	}

	if (t->p != NULL) {
		lua_pushinteger (L, t->p->id);
		lua_setfield (L, -2, "p");
	}


	lua_newtable (L);
	for (int i = 0; i < t->cs; i++) {

		lua_pushinteger(L, t->c[i]->id);
		lua_seti(L, -2, i + 1);
	}
	lua_setfield (L, -2, "c");

	return 1;
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

	tree_t *root = (tree_t *) lua_touserdata(L, -2);
	int vertically = lua_toboolean (L, -1);

	layout (root, vertically, NULL, NULL, NULL);

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
