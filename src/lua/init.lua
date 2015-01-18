fh = io.open("moo2d.wad", "rb")
moo2d = wad.createwad(fh:read("a"))
print(moo2d)