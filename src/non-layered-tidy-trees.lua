
local libluanonlayeredtidytrees = require 'libluanonlayeredtidytrees'

local nonlayeredtidytrees = {}

setmetatable (nonlayeredtidytrees, {__index = libluanonlayeredtidytrees})

function nonlayeredtidytrees.trees (rel)

    local tree = {}

    for k, each in pairs (rel) do tree[k] = libluanonlayeredtidytrees.mktree (k, each.w, each.h, each.y or 0.0, #each.c) end

    for k, parent in pairs (rel) do

        for i, child in ipairs (parent.c) do     -- really important to keep the order of children.

            libluanonlayeredtidytrees.atputchild (tree[k], i, tree[child])
        end
    end

    return tree
end

return nonlayeredtidytrees
