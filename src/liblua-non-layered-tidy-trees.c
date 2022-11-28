
/* 
	This is a glue c file for importing delta client c functions into Lua workflow.
*/


#ifdef _WIN32
	#define EXPORT __declspec(dllexport)
#else
	#define EXPORT __attribute__((visibility("default")))
#endif

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

	lua_Integer idx = lua_tointeger(L, -6);
	lua_Number w = lua_tonumber(L, -5);
	lua_Number h = lua_tonumber(L, -4);
	lua_Integer cs = lua_tointeger(L, -3);
	lua_Number x = lua_tonumber(L, -2);
	lua_Number y = lua_tonumber(L, -1);
	
	tree_t *t = (tree_t *) malloc (sizeof(tree_t));

	t->idx = idx;
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

	lua_pushinteger (L, t->idx);
	lua_setfield (L, -2, "idx");
	
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
		lua_pushlightuserdata (L, t->tl);
		lua_setfield (L, -2, "tl");
	}
	
	if (t->tr != NULL) {
		lua_pushlightuserdata (L, t->tr);
		lua_setfield (L, -2, "tr");
	}

	if (t->el != NULL) {
		lua_pushlightuserdata (L, t->el);
		lua_setfield (L, -2, "el");
	}

	if (t->er != NULL) {
		lua_pushlightuserdata (L, t->er);
		lua_setfield (L, -2, "er");
	}

	if (t->p != NULL) {
		lua_pushlightuserdata (L, t->p);
		lua_setfield (L, -2, "p");
	}

	lua_newtable (L);
	for (int i = 0; i < t->cs; i++) {
		lua_pushlightuserdata(L, t->c[i]);
		lua_seti(L, -2, i + 1);
	}
	lua_setfield (L, -2, "c");

	return 1;
}

static int l_dbindwhxy(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata (L, -1);

	lua_pushnumber (L, t->w);
	lua_pushnumber (L, t->h);
	lua_pushnumber (L, t->x);
	lua_pushnumber (L, t->y);
	
	return 4;
}

static void free_tree (tree_t *t) {

	for (int i = 0; i < t->cs; i++) free_tree (t->c[i]);

	free (t->c);
	free (t);
}

static int l_free(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata (L, -1);
	
	free_tree (t);

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

static void flatcoordinatesinto (tree_t *t, double *array) {

	for (int i = 0; i < t->cs; i++) flatcoordinatesinto (t->c[i], array);

	int idx = (t->idx - 1) * 4;

	array[idx] = t->x;
	array[idx + 1] = t->y;
	array[idx + 2] = t->w;
	array[idx + 3] = t->h;
}

static int l_flatcoordinatesinto(lua_State *L) {

	tree_t *t = (tree_t *) lua_touserdata(L, -2);
	double *a = (double *) lua_touserdata(L, -1);

	flatcoordinatesinto (t, a);

	return 0;
}

static void pairscb (tree_t *sr, tree_t *cl, double dist, void *userdata) {
	printf("(%d, %d): %lf, %lf\n", sr->idx, cl->idx, dist, 
		cl->prelim + cl->mod - (sr->prelim + sr->mod) );
}

static int l_layout(lua_State *L) {

	lua_getfield (L, -1, "root");
	tree_t *root = (tree_t *) lua_touserdata(L, -1);
	lua_pop(L, 1);

	lua_getfield (L, -1, "vertically");
	int vertically = lua_toboolean (L, -1);
	lua_pop(L, 1);

	lua_getfield (L, -1, "centeredxy");
	int centeredxy = lua_toboolean (L, -1);
	lua_pop(L, 1);

	layout (root, vertically, centeredxy, NULL, NULL, NULL, pairscb);

	return 0;
}

static const struct luaL_Reg tidytree_reg [] = {
	{"layout", l_layout},
	{"mktree", l_mktree},
	{"flatcoordinatesinto", l_flatcoordinatesinto},
	{"free", l_free},
	{"updatewh", l_updatewh},
	{"atputchild", l_atputchild},
	{"dbind", l_dbind},
	{"dbindwhxy", l_dbindwhxy},
	{NULL, NULL} /* sentinel */
};
 
EXPORT int luaopen_libluanonlayeredtidytrees (lua_State *L) {
	luaL_newlib(L, tidytree_reg);
	return 1;
}

int main () {}
