--[[
 - wadmake: a WAD manipulation utility.
 - Copyright (C) 2015  Alex Mayfield
 -
 - This program is free software: you can redistribute it and/or modify
 - it under the terms of the GNU General Public License as published by
 - the Free Software Foundation, either version 3 of the License, or
 - (at your option) any later version.
 -
 - This program is distributed in the hope that it will be useful,
 - but WITHOUT ANY WARRANTY; without even the implied warranty of
 - MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 - GNU General Public License for more details.
 -
 - You should have received a copy of the GNU General Public License
 - along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -]]

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

function Lumps:writewad(filename)
	local file = io.open(filename, 'wb')
	local data = self:packwad()
	file:write(data)
	file:close()
end

function Lumps:writezip(filename)
	local file = io.open(filename, 'wb')
	local data = self:packzip()
	file:write(data)
	file:close()
end

-- Additional functions for `wad` module
local mod = {}

function mod.readwad(filename)
	local file = io.open(filename, 'rb')
	local data = file:read('a')
	local lumps, type = wad.unpackwad(data)
	file:close()
	return lumps, type
end

function mod.readzip(filename)
	local file = io.open(filename, 'rb')
	local data = file:read('a')
	local lumps = wad.unpackzip(data)
	file:close()
	return lumps
end

return mod, Lumps
