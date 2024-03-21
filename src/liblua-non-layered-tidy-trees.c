

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <time.h>
#include <non-layered-tidy-trees.h>

typedef struct userdata_s
{
	lua_State *L;
	int contourpairscb_absidx;
} userdata_t;

int l_mktree(lua_State *L)
{
	lua_Integer idx = lua_tointeger(L, 1);
	lua_Number w = lua_tonumber(L, 2);
	lua_Number h = lua_tonumber(L, 3);
	lua_Integer cs = lua_tointeger(L, 4);
	lua_Number m = lua_tonumber(L, 5);

	tree_t *t = init_tree(idx, w, h, m, cs, false);

	lua_pushlightuserdata(L, t);

	return 1;
}

int l_atputchild(lua_State *L)
{

	tree_t *parent = (tree_t *)lua_touserdata(L, 1);
	int i = lua_tointeger(L, 2);
	tree_t *child = (tree_t *)lua_touserdata(L, 3);

	parent->c[i - 1] = child;
	child->p = parent;

	return 0;
}

int l_dbind(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, 1);

	lua_newtable(L);

	lua_pushinteger(L, t->idx);
	lua_setfield(L, -2, "idx");

	lua_pushinteger(L, t->level + 1);
	lua_setfield(L, -2, "level");

	lua_pushinteger(L, t->childno + 1);
	lua_setfield(L, -2, "childno");

	lua_pushboolean(L, t->centeredxy);
	lua_setfield(L, -2, "centeredxy");

	lua_pushnumber(L, t->margin);
	lua_setfield(L, -2, "margin");

	lua_pushboolean(L, t->isdummy);
	lua_setfield(L, -2, "isdummy");

	lua_pushnumber(L, t->w);
	lua_setfield(L, -2, "w");

	lua_pushnumber(L, t->h);
	lua_setfield(L, -2, "h");

	lua_pushnumber(L, t->x);
	lua_setfield(L, -2, "x");

	lua_pushnumber(L, t->y);
	lua_setfield(L, -2, "y");

	lua_pushnumber(L, t->prelim);
	lua_setfield(L, -2, "prelim");

	lua_pushnumber(L, t->mod);
	lua_setfield(L, -2, "mod");

	lua_pushnumber(L, t->shift);
	lua_setfield(L, -2, "shift");

	lua_pushnumber(L, t->change);
	lua_setfield(L, -2, "change");

	lua_pushnumber(L, t->msel);
	lua_setfield(L, -2, "msel");

	lua_pushnumber(L, t->mser);
	lua_setfield(L, -2, "mser");

	lua_pushinteger(L, t->cs);
	lua_setfield(L, -2, "cs");

	if (t->tl != NULL)
	{
		lua_pushlightuserdata(L, t->tl);
		lua_setfield(L, -2, "tl");
	}

	if (t->tr != NULL)
	{
		lua_pushlightuserdata(L, t->tr);
		lua_setfield(L, -2, "tr");
	}

	if (t->el != NULL)
	{
		lua_pushlightuserdata(L, t->el);
		lua_setfield(L, -2, "el");
	}

	if (t->er != NULL)
	{
		lua_pushlightuserdata(L, t->er);
		lua_setfield(L, -2, "er");
	}

	if (t->p != NULL)
	{
		lua_pushlightuserdata(L, t->p);
		lua_setfield(L, -2, "p");
	}

	lua_newtable(L);
	for (int i = 0; i < t->cs; i++)
	{
		lua_pushlightuserdata(L, t->c[i]);
		lua_seti(L, -2, i + 1);
	}
	lua_setfield(L, -2, "c");

	return 1;
}

int l_dbindwhxy(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, 1);

	int ret = 4;
	if (lua_checkstack(L, ret))
	{
		lua_pushnumber(L, t->w);
		lua_pushnumber(L, t->h);
		lua_pushnumber(L, t->x);
		lua_pushnumber(L, t->y);
	}
	else
	{
		ret = 0;
	}

	return ret;
}

int l_free(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, 1);

	free_tree(t);

	return 0;
}

int l_updatewh(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, 1);
	lua_Number w = lua_tonumber(L, 2);
	lua_Number h = lua_tonumber(L, 3);

	t->w = w;
	t->h = h;

	return 0;
}

int l_flat_xywh_into(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, 1);
	double *a = (double *)lua_touserdata(L, 2);

	flat_xywh_into(t, a);

	return 0;
}

int l_flat_xy_into(lua_State *L)
{

	int n = lua_tointeger(L, 1);
	tree_t **nodes = (tree_t **)lua_touserdata(L, 2);
	double *xy = (double *)lua_touserdata(L, 3);

	flat_xy_into(n, nodes, xy);

	return 0;
}

void pairscb(tree_t *sr, tree_t *cl, double dist, void *userdata)
{
	userdata_t *ud = (userdata_t *)userdata;

	if (ud->contourpairscb_absidx != -1)
	{

		lua_State *L = ud->L;

		lua_pushvalue(L, ud->contourpairscb_absidx);
		lua_pushlightuserdata(L, sr);
		lua_pushlightuserdata(L, cl);
		lua_pushnumber(L, dist);

		lua_call(L, 3, 0);
	}

	/*
	printf("(%d (el: %d, er: %d, tl: %d, tr: %d), %d (el: %d, er: %d, tl: %d, tr: %d)): %lf, %lf\n",
			sr->idx,
			sr->el ? sr->el->idx: -1, sr->er ? sr->er->idx: -1,
			sr->tl ? sr->tl->idx: -1, sr->tr ? sr->tr->idx: -1,


			cl->idx,
			cl->el ? cl->el->idx: -1, cl->er ? cl->er->idx: -1,
			cl->tl ? cl->tl->idx: -1, cl->tr ? cl->tr->idx: -1,

			dist, cl->prelim + cl->mod - (sr->prelim + sr->mod) );
	*/
}

int l_layout(lua_State *L)
{

	treeinput_t input;
	userdata_t ud;

	ud.L = L;
	ud.contourpairscb_absidx = -1;

	lua_getfield(L, -1, "root");
	input.t = (tree_t *)lua_touserdata(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "vertically");
	input.vertically = lua_toboolean(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "centeredxy");
	input.centeredxy = lua_toboolean(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "x");
	input.x = lua_isnumber(L, -1) == 1 ? lua_tonumber(L, -1) : 0.0;
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	input.y = lua_isnumber(L, -1) == 1 ? lua_tonumber(L, -1) : 0.0;
	lua_pop(L, 1);

	lua_getfield(L, -1, "contourpairscb");
	if (lua_isfunction(L, -1) == 1)
	{
		ud.contourpairscb_absidx = lua_absindex(L, -1);

		input.cpairscb = pairscb;
		input.cpairsud = &ud;
	}
	else
	{
		input.cpairscb = input.cpairsud = NULL;
	}

	input.walkcb = input.walkud = NULL;

	layout(&input);

	lua_pop(L, 1); // then pop contourpairscb or nil.

	return 0;
}

int l_maxbottombetween(lua_State *L)
{

	tree_t *from = (tree_t *)lua_touserdata(L, -3);
	tree_t *to = (tree_t *)lua_touserdata(L, -2);
	int vertically = lua_toboolean(L, -1);

	fringemaxbottom_t ud;
	ud.bottom = bottom(from->p, vertically);
	ud.vertically = vertically;

	maxbottombetween(from, to, &ud);

	lua_pushnumber(L, ud.bottom);

	return 1;
}

int l_bottom(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -2);
	int vertically = lua_toboolean(L, -1);

	double b = bottom(t, vertically);

	lua_pushnumber(L, b);

	return 1;
}

int l_reifyflatchunks(lua_State *L)
{

	int n = lua_tointeger(L, -5); // total number of nodes.
	double *wh = (double *)lua_touserdata(L, -4);
	double *whg = (double *)lua_touserdata(L, -3);
	int *children = (int *)lua_touserdata(L, -2);
	int rooti = lua_tointeger(L, -1) - 1; // Lua works in 1-based indexing therefore shift back by 1.

	tree_t *root;
	tree_t **nodes = reifyflatchunks(n, wh, whg, children, rooti, &root);

	lua_pushlightuserdata(L, root);	 // push the root node.
	lua_pushlightuserdata(L, nodes); // push also the whole array to speed up the fetching phase.

	return 2;
}

const struct luaL_Reg tidytree_reg[] = {
	{"layout", l_layout},
	{"mktree", l_mktree},
	{"flat_xywh_into", l_flat_xywh_into},
	{"flat_xy_into", l_flat_xy_into},
	{"free", l_free},
	{"updatewh", l_updatewh},
	{"atputchild", l_atputchild},
	{"dbind", l_dbind},
	{"maxbottombetween", l_maxbottombetween},
	{"dbindwhxy", l_dbindwhxy},
	{"bottom", l_bottom},
	{"reifyflatchunks", l_reifyflatchunks},
	{NULL, NULL} /* sentinel */
};

int luaopen_libluanonlayeredtidytrees(lua_State *L)
{
	luaL_newlib(L, tidytree_reg);
	return 1;
}
