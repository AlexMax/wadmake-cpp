-- Additional functions for the `Lumps` userdata
local Lumps = {}

-- Copy a range of lumps from one Lumps to another
function Lumps.move(source, istart, iend, ito, dest)
	if dest == nil then
		dest = source
	end
	for i = istart, iend do
		source:set(ito + istart - i, source:get(i))
	end
	return dest
end

-- Additional functions for `wad` module
local mod = {}

function mod.openwad(filename)
	local file = io.open(filename, 'rb')
	local data = file:read('a')
	return wad.readwad(data)
end

function mod.openzip(filename)
	local file = io.open(filename, 'rb')
	local data = file:read('a')
	return wad.readzip(data)
end

return mod, Lumps