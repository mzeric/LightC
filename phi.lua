#!/usr/bin/env lua

print ("Lua ...")

local a={}
a[1]=4
a[2]=1
a[3] = 4
a[4] = 2
a[5] = 2

local c = {}
c[2] = 3
c[4] = 1
local b={}
function table.first(_table)
	for ik, iv in pairs(_table) do
		return iv
	end
end
function table.length( t )
	-- body
	local count = 0
	if not type(t) == 'table' then
		return 0
	end
	for _,n in pairs(t) do
		count = count + 1;
	end
	return count
end

function build (a)
    local gra_out = {}

    --compute every out-degree
    for k,v in pairs(a) do
        local from = v
        local to = k
        if not gra_out[from] then gra_out[from] = {} end
        if not gra_out[to] then gra_out[to] = {} end
        print ("mov "..v.." to "..k)
        gra_out[from][#gra_out[from]+1] = to
    end
    for k,v in pairs(gra_out) do
        print ("from "..k.." to")
        for _,d in pairs(v) do
            print(d.." ")
        end

    end
    return gra_out

end

function find_next(b) 
    for k,v in pairs(b) do
        if table.length(v) == 0 then
            return k
        end
    end

    return nil
end
function reduce (graph, x)
    graph[x] = nil
    for k, v in pairs (graph) do
        for m, n in pairs(v) do
            if n == x then
            v[m] = nil
            --print ("graph["..k.."]"..n.."=nil")
            end
         end
     end
    
end
function out_mov(from, to)
	print ("mov "..from.." -> "..to)
end
function topological_phi(graph, b)

    local tmp_count = 1
    local break_cycle = nil
    local break_next = nil
    while(table.length(b) > 0) do
	    local n = find_next(b)
	    if not n then
	    	    -- break the cycle with temp var
	    	    for ik, iv in pairs(b) do
	    	    	    break_cycle = ik
	    	    	    break
		    end

		    -- break
		    break_next = table.first(b[break_cycle])
	    	    out_mov(break_cycle, "T"..tmp_count)
	    	    reduce(b, break_next)

		    --n = find_next(b)
		    topological_phi(graph, b)
	        out_mov("T"..tmp_count, break_next)
	    end
	    if (not graph[n]) then
	    	    -- n is last of the cluter
	    else
		    out_mov(graph[n], n)
	    end
	    if n then
		    reduce(b, n)
	    end

    end
    if(break_next) then
--	    out_mov("T"..tmp_count, break_next)
    end

end



local b = build(a)
topological_phi(a, b)
