Scripting Reference
*******************

Although WADmake is a tool designed to make WAD management simple, that is not
the end of the story.  If you have needs that go beyond the scope of the
built-in declrarations of a wadmake.lua file, you can harness the power of the
exact same functions that WADmake uses under the hood for your own purposes.
You can augment your wadmake.lua file, or you can create a free-form script that
does something completely different.

WADmake is built on top of Lua 5.3.  If you are a complete beginner to
programming, or if you could use a refresher, I highly recommend Programming in
Lua, which can be found `here <http://www.lua.org/pil/contents.html>`_.  If you
are already somewhat familiar with programming, you can likely get away with
reading a quick 15-minute introduction to the language
`here <http://tylerneylon.com/a/learn-lua/>`_.  The official complete reference
to the language, including all of the built-in modules, can be found
`here <http://www.lua.org/manual/5.3/>`_.

wad
===

The wad module contains some useful top-level functions.

.. function:: createLumps()
   :module: wad

   Create an empty Lumps userdata.

.. function:: openwad(filename)
   :module: wad

   Returns a Lumps userdata created from the passed WAD file, and a string
   signifying if the file was an ``iwad`` or a ``pwad``.

.. function:: openzip(filename)
   :module: wad

   Returns a Lumps userdata created from the passed ZIP file.

.. function:: unpackwad(data)
   :module: wad

   Returns a Lumps userdata created from the passed raw WAD file data, and a
   string signifying if the file was an ``iwad`` or a ``pwad``.

.. function:: unpackzip(data)
   :module: wad

   Returns a Lumps userdata created from the passed raw ZIP file data.

Lumps
=====

Lumps userdata are the fundamental building blocks of WADmake.  It represents
an ordered directory of lumps with names and values.

.. function:: find(name[, start])
   :module: Lumps

   Does a linear search through all lumps, looking for a lump with a specific
   name.  If start is supplied, the search starts at that index.

   Returns the index of the lump, or nil if the lump was not been found.

   This search is case-sensitive.

.. function:: get(index)
   :module: Lumps

   Returns the name and contents of a lump at a given lump index.

.. function:: insert([index, ]name, data)
   :module: Lumps

   Inserts a lump with a given name and data at the given index.  The lump
   currently at the given index and all lumps therafter are moved up one index.
   If index is omitted, the lump is appended to the end.

.. function:: move(start, end, dstart[, destination])
   :module: Lumps

   Copies lumps between the start and end indexes to a destination Lumps
   userdata, starting at the dstart index.  If destination is nil or omitted,
   the destination Lumps will be the source Lumps.

   In other words::

       src:move(3, 5, 4, dest)

   ...will copy indexes 3 through 5 in src to dest starting at index 4 and
   ending at index 6.

.. function:: packwad()
   :module: Lumps

   Returns a string containing the raw WAD data of the underlying Lumps
   userdata.

.. function:: packzip()
   :module: Lumps

   Returns a string containing the raw ZIP data of the underlying Lumps
   userdata.

.. function:: remove(index)
   :module: Lumps

   Removes a lump from the given index.

.. function:: set(index, name[, data])
   :module: Lumps

   Sets the lump name and data at a given index.  If name is set to nil, the
   old name is kept.  If data is set to nil or omitted, the old contents of the
   lump is kept.

.. function:: writewad(filename)
   :module: Lumps

   Write a WAD file to disk using the given filename.  If the file already
   exists, it will be overwritten.

.. function:: writezip(filename)
   :module: Lumps

   Write a ZIP file to disk using the given filename.  If the file already
   exists, it will be overwritten.
