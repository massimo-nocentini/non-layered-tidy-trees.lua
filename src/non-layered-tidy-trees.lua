
local libluanonlayeredtidytrees = require 'libluanonlayeredtidytrees'

local nonlayeredtidytrees = {}

setmetatable (nonlayeredtidytrees, {__index = libluanonlayeredtidytrees})

function nonlayeredtidytrees.trees (rel)

    local tree, itree = {}, {}

    if false and rel.addspacers then

        rel.addspacers = nil    -- remove from the input table.

        local newrel = {}

        for k, each in pairs (rel) do

            if k <= 0 then error 'Use positive keys please.' end

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
        tree[k] = libluanonlayeredtidytrees.mktree (each.w, each.h, #each.c, each.x or 0.0, each.y or 0.0)
        itree[tree[k]] = k
    end

    for k, parent in pairs (rel) do

        for i, child in ipairs (parent.c) do     -- really important to keep the order of children.

            libluanonlayeredtidytrees.atputchild (tree[k], i, tree[child])
        end
    end

    return tree, itree
end

function nonlayeredtidytrees.positions (itree)
    
    local destructured = {}

    for t, k in pairs (itree) do 
        local w, h, x, y = nonlayeredtidytrees.dbindwhxy (t)
        destructured[k] = {w=w, h=h, x=x, y=y}
    end
    
    return destructured
end

function nonlayeredtidytrees.dbindrec (itree)
    
    local destructured = {}

    for t, k in pairs (itree) do 
        destructured[k] = nonlayeredtidytrees.dbind (t)
        destructured[k].id = k
    end

    for k, tbl in pairs (destructured) do
        local c = {  }
        for i, ud in pairs (tbl.c) do
            c[i] = destructured[itree[ud]]
        end
        tbl.c = c

        --if tbl.p then tbl.p = destructured[itree[tbl.p]] end
        
    end
    
    return destructured
end

return nonlayeredtidytrees
