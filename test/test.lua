
local lu = require 'luaunit'
local tidy = require 'non-layered-tidy-trees'

function test_simple ()



	local tree, itree = tidy.trees {

        addspacers = false,

        [1] = {w = 70, h = 30, c = {2, -3, 4, 8}},
        [2] = {w = 10, h = 50, c = {}},
        [-3] = {w = 10, h = 100, c = { 3 } },
        [3] = {w = 20, h = 30, c = {}},
        [4] = {w = 30, h = 30, c = {5, 7}},
        [5] = {w = 20, h = 50, c = {6}},
        [6] = {w = 40, h = 10, c = {}},
        [7] = {w = 20, h = 20, c = {}},
        [8] = {w = 30, h = 50, c = {9, 10}},
        [9] = {w = 30, h = 20, c = {}},
        [10] = {w = 10, h = 10, c = {}},

    }

    local r = tree[1]

    tidy.layout {
		root = r,
		vertically = false,
		centeredxy = true,
	 }

    local result = tidy.dbindrec (itree)

    tidy.free(r);


	lu.assertEquals(result, {})


end

os.exit( lu.LuaUnit.run() )
