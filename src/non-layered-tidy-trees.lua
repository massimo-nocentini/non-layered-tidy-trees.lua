
local libluanonlayeredtidytrees = require 'libluanonlayeredtidytrees'

local nonlayeredtidytrees = {}

setmetatable (nonlayeredtidytrees, {__index = libluanonlayeredtidytrees})

function nonlayeredtidytrees.trees (rel)

    local tree, itree = {}, {}

    do
        local idx = 1
        for k, each in pairs (rel) do 
            tree[k] = libluanonlayeredtidytrees.mktree (each.idx or idx, each.w, each.h, #each.c, each.m or 0.0)
            itree[tree[k]] = k
            idx = idx + 1
        end
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

function nonlayeredtidytrees.dbindrec (itree, externalkey)
    
    local destructured = {}

    for t, k in pairs (itree) do 
        local tdest = nonlayeredtidytrees.dbind (t)
        if externalkey then tdest[externalkey] = k end
        destructured[k] = tdest
    end

    for k, tbl in pairs (destructured) do
        local c = {  }
        for i, ud in pairs (tbl.c) do c[i] = itree[ud] end
        tbl.c = c

        local p = tbl.p; if p then tbl.p = itree[p] end
        local el = tbl.el; if el then tbl.el = itree[el] end
        local er = tbl.er; if er then tbl.er = itree[er] end
        local tl = tbl.tl; if tl then tbl.tl = itree[tl] end
        local tr = tbl.tr; if tr then tbl.tr = itree[tr] end
        
    end
    
    return destructured
end

return nonlayeredtidytrees
