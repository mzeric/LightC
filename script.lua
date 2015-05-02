#!/usr/bin/env lua

print ("Execute Lua Script .........")
--[[for k,v in pairs(a) do
	print ("test ".. type(v)..#v)
	print ("t "..v[1]..v[2])
	
end]]
function calc_a(p, x)
	local sum=0;
	local mul={}
	mul[1] = x;
	mul[0] = 1;
	for i=2, #p do
		mul[i] = mul[i-1]*x
	end

	for i,v in pairs(p) do
		sum = sum + mul[i-1]*v
	end
	return sum
end
p={1, 3 , 57, 7}
print (calc_a(p, 3.1415926))
function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end
function true2set(a)
	local t = {}
	for k,_v in pairs(a) do
		if _v then
			t[#t+1] = k
		end
	end
	return t
end
function set_in(a, e)

	for k,v in pairs(a) do
		if v == e then
			return true			
		end
	end
	return false
end
function set_remove(a, e)
	for k,v in pairs(a) do
		if v == e then
			a[k] = nil
		end
	end
end
function set_comm(a, b)
	local t = {}
	local ret = {}
	for _,k in pairs(a) do
		t[k] = true
	end
	for _,k in pairs(b) do
		if t[k] == true then
			ret[#ret+1] = k
		end
	end
	return ret
end

function set_union(a, b)
	local t = {}
	for _,k in pairs(a) do
		t[k] = true
	end
	for _,k in pairs(b) do
		t[k] = true
	end

	return true2set(t)
end

function set_equal(a, b)
	local t = {}
	for _,k in pairs(a) do
		t[k] = true;
	end
	for _,k in pairs(b) do
		if t[k] == true then
			t[k] = nil
		else
			return false
		end
	end
	for _,k in pairs(t) do
		if t[k] == true then
			return false
		end
	end
	return true
end
-- cfg = { {from, to}, }
-- get -> cfg[from] = {to}
function cfg_input(data)
	local cfg_out = {}
	for _,v in pairs(data) do
		if type(v) == 'table' and #v == 2 then
			-- should be
			local from = v[1]
			local to   = v[2]
			local _from = cfg_out[from]
			if not _from then cfg_out[from] = {} end
			print ("from "..from.." to "..to)
			
			cfg_out[from][#cfg_out[from] + 1] = to
		end
	end
	return cfg_out
end
function  cfg_nodes(cfg)
	-- get all cfg node number ,just in case not in consistent
	local all = {}
	local ret = {}
	all[0] = true
	for from, to in pairs(cfg) do
		for _,v in pairs(to) do
			all[v] = true
		end
	end
	for k,v in pairs(all)do
		if v == true then
			ret[#ret+1] = k
		end
	end

	return ret
end

function cfg_succ(cfg, n)
	return cfg[n]
end
function cfg_pred(cfg, n)
	local pred = {}
	
	for from, to in pairs(cfg) do
		for _, t in pairs(to) do
			if t == n then
				pred[#pred+1] = from
				break
			end
		end
	end
	return pred
end

-- from 0 -> n, 0 = Entry
function dom_build(cfg)
	local dom= {};
	local all_node = cfg_nodes(cfg)
	
	for _,i in pairs(all_node) do
		dom[i] = all_node
	end
	dom[0] = {0}

	
	local changed = true

	while changed do
		changed = false

		for _,note in pairs(all_node) do
			if note == 0 then -- exclude ENTRY node
				goto continue
			end

			local old = deepcopy(dom[note])
			local preds = cfg_pred(cfg, note)
			local comms = dom[preds[#preds]]
			for _,k in pairs(preds) do
				comms = set_comm(comms, dom[k])
			end
			dom[note] = set_union(comms, {note})
		
			if not set_equal(dom[note], old)then
				changed = true
			end

			::continue::

		end
	end
	return dom
end


-- 根据定义 idom = dom - self - 相互有dom关系的
function idom_build(doms, all_node)
	local idom = {}
	local sdom = {}
	local ret = {}
	
	idom[0] = {0}
	for k,v in pairs(doms) do

		sdom[k] = deepcopy(doms[k]) -- sdom hold still during the algorithm
		set_remove(sdom[k], k)

		idom[k] = deepcopy(sdom[k])
	end
	
	for _,t in pairs(all_node) do
		for _,x in pairs(idom[t]) do
			for k,y in pairs(idom[t]) do
				if y ~= x then
					if set_in(sdom[x], y) then
						idom[t][k] = nil -- remove y as y sdom x
					end				
				end
			end		
		end
	end
	for k,v in pairs(idom) do
		for _,j in pairs(v) do
			ret[k] = j
		end
	end

	return ret
end
-- new way to build idom and dominance frontier
function idom_build2()

end
function D_tree_child(idom, n)
	local S = {}
	for k,v in pairs(idom)do

		if v == n then
			S[#S+1] = k
		end
	end
	return S
end
function df_build(cfg, dom, idom, n)

	local S = {}
	local succs = cfg_succ(cfg, n)
	if not succs then return {} end
	for _,y in pairs(cfg_succ(cfg, n)) do

		if idom[y] ~= n then
			S[#S+1] = y
		end
		::continue::
	end
	for _,c in pairs(D_tree_child(idom, n)) do
		
		local dc = df_build(cfg, dom, idom, c)
		
		for _,w in pairs(dc) do

			if n == w or not set_in(dom[w], n) then

				S[#S+1] = w
			end
		end

	end

	return S

end
function DF_build(cfg)
	-- body
	local dom = dom_build(cfg)
	local all_node = cfg_nodes(cfg)
	local idom = idom_build(dom, all_node)
	
	local DF={}

	for _,v in pairs(all_node) do
		c = df_build(cfg, dom, idom, v)
		DF[v] = c
	end
	return DF
end


--cfg_ = {{0,1},{1,2},{1,3},{2,4},{2,5},{4,6},{5,6},{6,1}}
--cfg_ = {{0,0},{0,2},{2,11},{2,4},{4,5},{5,7},{5,9},{7,9},{9,13},{13,15},{11,13}}
print ("test "..type(cfg_))
cfg = cfg_input(cfg_)
--[[
for from, to in pairs(cfg) do
	print ("from :" .. from)
	for _,v in pairs(to) do
		print (v)
	end
end
]]

it2 = DF_build(cfg)
--t = set_comm({6,2,3,4},{2,4,5,6})
--t = {{1,2},3}
print ("Dominance Frontier (D-F) is")
for n,k in pairs(it2) do
	print ("node :" .. n)
	for _,v in pairs(k) do
		print (v)
	end
end


function vars_live_in( block )
        -- vars that live ath entry of block
end
function vars_live_out( block )
        --[[
        lj;kl]]--
end

print (math.sin(2))
function defs_of_var( var )
	-- all define sites of a var
	-- stmts assign the var
end
function vars_need_phi( cfg )
	-- all vars
	-- vars live at exit of block
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
function place_phi( cfg )
        -- body
        local all_node = cfg_nodes(cfg)
        local DF = DF_build(cfg)
        local PHI={}
        for node in pairs(all_node) do end
        for _,var in pairs(vars_need_phi(cfg)) do
        	local w = defs_of_var(var)
        	local orig = deepcopy(w)
        	while(table.length(w)) do
        		local n = table.remove(w, 1) -- get the head element
        		for _,y in pairs(DF[n]) do
        			if not set_in(PHI[var], y) then
        				-- insert phi at top of block[y]
        				table.insert(PHI[var],y) -- append
        				if not set_in(orig, y) then
        					table.insert(w, y)
        				end
        			end
        		end
        	end
        end

end

function get_def_use( program )
	-- cfg

end



