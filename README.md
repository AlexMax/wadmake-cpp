WADmake
=======

A utility for creating WAD, ZIP (PK3) and 7Z (PK7) archives for Doom.

FAQ
---

Q: Why?

A: Because deutex needed to be put out of its misery.

Q: Why not SLADE/XWE?

A: SLADE operates on a single monolithic file, which cannot be version-controlled in a useful fashion.  Revision control has been standard practice in the software field for at least a decade, and the benefits of using revision control software are too numerous to list here.

Q: Why not use zip/7za in a shell script or makefile?

A: Aside from natively supporting WAD files as output, WADmake can automatically compile ACS libraries, compress PNG files, dedupe textures, and other such things automatically.  In theory, you could create a batch file or shell script to do this for you, but WADmake makes it easy, and is cross-platform to boot. **NOTE: Doesn't actually do any of this yet, still working on it**

Q: Why not several dozen single-use utilities?

A: Because I wanted the utility to be a single executable that you could copy around freely and drop into a project, like deutex.  Of course, there are still some operations that require third party utilities, but such utilities are optional if you do not require their functionality.

Q: Why Lua?

A: A build system should be extensible, and I did not want to invent a new extension language from scratch.  In addition to its small size and ubiquity as an extension language, Lua seemed like a natrual fit as it originated as a configuration language and has been used in other build systems like Premake.

Contributing
------------

Contributions are welcome.  Here are the project's standards:

- Use 1TBS brace style.
- Indent with tabs, align with spaces.
- Your code should not generate compiler warnings.
- Please use a tool that checks for undefined behavior and memory leaks, such as Address Sanitizer, Valgrind or Dr. Memory.
- Keep in mind that all libraries are to be statically-linked into the executable on Windows.  No outside .dll files allowed.

License
-------

Currently [GNU GPLv3](http://www.gnu.org/licenses/gpl.html).  If there is a compelling case to be made for a more permissive license, I am open to suggestions.
