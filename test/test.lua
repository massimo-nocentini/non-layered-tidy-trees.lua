
local lu = require 'luaunit'
local tidy = require 'non-layered-tidy-trees'

function est_simple_input ()

    local tree = tidy.trees {

        [0] = {w = 70, h = 30, c = {1, 2, 3, 7}},
        [1] = {w = 10, h = 50, c = {}},
        [2] = {w = 20, h = 30, c = {}},
        [3] = {w = 30, h = 30, c = {4, 6}},
        [4] = {w = 20, h = 50, c = {5}},
        [5] = {w = 40, h = 10, c = {}},
        [6] = {w = 20, h = 20, c = {}},
        [7] = {w = 30, h = 50, c = {8, 9}},
        [8] = {w = 30, h = 20, c = {}},
        [9] = {w = 10, h = 10, c = {}},

    }

    local root = tree[0]

    --tidy.layout (root)

    local result = {}

    for k, t in pairs (tree) do

        local tbl = tidy.dbind (t)

        lu.assertEquals (k, tbl.id)
        result[k] = tbl

    end

    tidy.free(root);

    lu.assertEquals (result, {})



end

function test_simple ()

    local tree = tidy.trees {

        [1] = {w = 70, h = 30, c = {2, 3, 4, 8}},
        [2] = {w = 10, h = 50, c = {}},
        [3] = {w = 20, h = 30, c = {}},
        [4] = {w = 30, h = 30, c = {5, 7}},
        [5] = {w = 20, h = 50, c = {6}},
        [6] = {w = 40, h = 10, c = {}},
        [7] = {w = 20, h = 20, c = {}},
        [8] = {w = 30, h = 50, c = {9, 10}},
        [9] = {w = 30, h = 20, c = {}},
        [10] = {w = 10, h = 10, c = {}},

    }

    local root = tree[1]

    tidy.layout (root)

    local result = {}

    for k, t in pairs (tree) do

        local tbl = tidy.dbind (t)

        lu.assertEquals (k, tbl.id)
        result[k] = tbl

    end

    tidy.free(root);

    lu.assertEquals (result, {})



end

os.exit( lu.LuaUnit.run() )
