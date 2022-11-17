
local libluanonlayeredtidytrees = require 'libluanonlayeredtidytrees'

local nonlayeredtidytrees = {}

local input = {

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

function nonlayeredtidytrees.trees (rel)

    local tree = {}

    for k, each in pairs (rel) do tree[k] = libluanonlayeredtidytrees.mktree (k, each.w, each.h, #each.c) end

    for k, parent in pairs (rel) do

        for i, child in ipairs (parent.c) do     -- really important to keep the order of children.

            libluanonlayeredtidytrees.atputchild (tree[k], i, tree[child])
        end
    end

    return tree
end

function nonlayeredtidytrees.sort (tbl, C, reverse)

    reverse = reverse or false
    C = C or function (a, b) return a < b end
    return libnonlayeredtidytrees.sort(tbl, reverse, C)

end

return nonlayeredtidytrees
