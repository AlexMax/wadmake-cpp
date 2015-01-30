-- Additional functions for `wad` module
local mod = {}

function mod.openwad(filename)
	local file = io.open(filename, 'rb')
	local data = file:read('a')
	return wad.readwad(data)
end

return mod