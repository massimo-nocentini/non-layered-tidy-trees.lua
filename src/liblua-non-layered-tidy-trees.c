
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

typedef struct userdata_s
{
	lua_State *L;
	int contourpairscb_absidx;
} userdata_t;

static void init_tree(tree_t *t, int idx, double w, double h, double m, int cs, int isdummy)
{
	t->idx = idx;
	t->w = w;
	t->h = h;
	t->margin = m;
	t->isdummy = isdummy;

	t->x = 0.0;
	t->y = 0.0;
	
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

	t->p = NULL;

	t->cs = cs;
	t->c = cs == 0 ? NULL : (tree_t **)malloc(sizeof(tree_t *) * cs);

	/* Those two will be computed by the algorithm. */
	t->level = -1;
	t->childno = -1;
	t->centeredxy = -1;
}

static int l_mktree(lua_State *L)
{

	lua_Integer idx = lua_tointeger(L, -5);
	lua_Number w = lua_tonumber(L, -4);
	lua_Number h = lua_tonumber(L, -3);
	lua_Integer cs = lua_tointeger(L, -2);
	lua_Number m = lua_tonumber(L, -1);

	tree_t *t = (tree_t *)malloc(sizeof(tree_t));

	init_tree(t, idx, w, h, m, cs, 0);

	lua_pushlightuserdata(L, t);

	return 1;
}

static int l_atputchild(lua_State *L)
{

	tree_t *parent = (tree_t *)lua_touserdata(L, -3);
	int i = lua_tointeger(L, -2);
	tree_t *child = (tree_t *)lua_touserdata(L, -1);

	parent->c[i - 1] = child;
	child->p = parent;

	return 0;
}

static int l_dbind(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -1);

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

static int l_dbindwhxy(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -1);

	lua_pushnumber(L, t->w);
	lua_pushnumber(L, t->h);
	lua_pushnumber(L, t->x);
	lua_pushnumber(L, t->y);

	return 4;
}

static int l_free(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -1);

	free_tree(t);

	return 0;
}

static int l_updatewh(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -3);
	lua_Number w = lua_tonumber(L, -2);
	lua_Number h = lua_tonumber(L, -1);

	t->w = w;
	t->h = h;

	return 0;
}

static void flat_xywh_into(tree_t *t, double *array)
{

	for (int i = 0; i < t->cs; i++)
		flat_xywh_into(t->c[i], array);

	int idx = (t->idx - 1) * 4;

	array[idx] = t->x;
	array[idx + 1] = t->y;
	array[idx + 2] = t->w;
	array[idx + 3] = t->h;
}

static int l_flat_xywh_into(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -2);
	double *a = (double *)lua_touserdata(L, -1); // an array of doubles, really.

	flat_xywh_into(t, a);

	return 0;
}

static int l_flat_xy_into(lua_State *L)
{

	int n = lua_tointeger(L, -3);
	tree_t **nodes = (tree_t **)lua_touserdata(L, -2);
	double *xy = (double *)lua_touserdata(L, -1);

	tree_t *node;

	for (int i = 0; i < n; i++)
	{

		node = nodes[i];

		xy[node->idx - 1] = node->x;
		xy[node->idx - 1 + n] = node->y;
	}

	return 0;
}

static void pairscb(tree_t *sr, tree_t *cl, double dist, void *userdata)
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

static int l_layout(lua_State *L)
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
	input.x = lua_isnumber(L, -1) == 1 ? lua_tonumber (L, -1) : 0.0;
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	input.y = lua_isnumber(L, -1) == 1 ? lua_tonumber (L, -1) : 0.0;
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

typedef struct fringemaxbottom_s
{
	double bottom;
	int vertically;
} fringemaxbottom_t;

static double maxbottom(tree_t *t, tree_t *to, int vertically, int *found)
{

	double b;
	double m = bottom(t, vertically);

	for (int i = 0; !*found && i < t->cs; i++)
	{
		tree_t *child = t->c[i];
		if (child == to)
		{
			*found = 1;
			return m;
		}
		b = maxbottom(child, to, vertically, found);
		m = b > m ? b : m;
	}

	return m;
}

static void maxbottombetween(tree_t *from, tree_t *to, fringemaxbottom_t *ud)
{

	double b;
	tree_t *p = from->p;

	// assert (p != NULL);
	if (p == NULL)
		return;

	int found = 0;

	for (int i = from->childno + 1; !found && i < p->cs; i++)
	{

		tree_t *child = p->c[i];

		if (child == to)
		{
			found = 1;
			break;
		}

		b = maxbottom(child, to, ud->vertically, &found);

		ud->bottom = b > ud->bottom ? b : ud->bottom;
	}

	if (!found)
		maxbottombetween(p, to, ud);
}

static int l_maxbottombetween(lua_State *L)
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

static int l_bottom(lua_State *L)
{

	tree_t *t = (tree_t *)lua_touserdata(L, -2);
	int vertically = lua_toboolean(L, -1);

	double b = bottom(t, vertically);

	lua_pushnumber(L, b);

	return 1;
}

static int l_reifyflatchunks(lua_State *L)
{

	int n = lua_tointeger(L, -5); // total number of nodes.
	double *wh = (double *)lua_touserdata(L, -4);
	double *whg = (double *)lua_touserdata(L, -3);
	int *children = (int *)lua_touserdata(L, -2);
	int rooti = lua_tointeger(L, -1) - 1; // Lua works in 1-based indexing therefore shift back by 1.

	int nedges, i, j;

	tree_t *node; // auxiliary variable to reference newly allocated memory locations.
	tree_t **nodes = (tree_t **)malloc(sizeof(tree_t *) * n * 2);

	for (i = 0; i < n; i++)
	{
		node = (tree_t *)malloc(sizeof(tree_t));
		init_tree(node, i + 1, wh[i], wh[i + n], wh[i + 2 * n], children[i], 0);
		nodes[i] = node; // the node with the content.

		node = (tree_t *)malloc(sizeof(tree_t));
		init_tree(node, i + 1 + n, whg[i], whg[i + n], 0.0, 1, 1);
		node->c[0] = nodes[i];
		nodes[i + n] = node; // the node that separates.
	}

	for (i = 0, nedges = n; i < n; i++)
		for (node = nodes[i], j = 0; j < node->cs; j++, nedges++)
			node->c[j] = nodes[children[nedges] - 1 + n];

	// node = (tree_t *) malloc (sizeof (tree_t));
	// memcpy (node, nodes[rooti], sizeof (tree_t));

	// free (nodes);	// release the memory that keeps the whole array but not each individual node.

	lua_pushlightuserdata(L, nodes[rooti + n]); // push the root node.
	lua_pushlightuserdata(L, nodes);			// push also the whole array to speed up the fetching phase.

	return 2;
}

static const struct luaL_Reg tidytree_reg[] = {
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

EXPORT int luaopen_libluanonlayeredtidytrees(lua_State *L)
{
	luaL_newlib(L, tidytree_reg);
	return 1;
}
