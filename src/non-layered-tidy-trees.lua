
local libluanonlayeredtidytrees = require 'libluanonlayeredtidytrees'

local nonlayeredtidytrees = {}

setmetatable (nonlayeredtidytrees, {__index = libluanonlayeredtidytrees})

function nonlayeredtidytrees.trees (rel)

    local tree = {}

    if rel.addspacers then

        rel.addspacers = nil    -- remove from the input table.

        local newrel = {}

        for k, each in pairs (rel) do

            local newc = {}
            for i, c in ipairs (each.c) do newc[i] = -c end
            local neweach = {w = each.w, h = each.h, y = each.y, c = newc }
            newrel[k] = neweach
            newrel[-k] = { w = 0, h = 1, y = 0, c = { k } }
        end

        rel = newrel
    end

    rel.addspacers = nil    -- be sure that the key `addspacers` is removed even if it is `false`.

    for k, each in pairs (rel) do 
        tree[k] = libluanonlayeredtidytrees.mktree (k, each.w, each.h, each.x or 0, each.y or 0.0, #each.c)
    end

    for k, parent in pairs (rel) do

        for i, child in ipairs (parent.c) do     -- really important to keep the order of children.

            libluanonlayeredtidytrees.atputchild (tree[k], i, tree[child])
        end
    end

    return tree
end

return nonlayeredtidytrees
