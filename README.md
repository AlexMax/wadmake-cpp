WADmake
=======

A utility for creating WAD, ZIP (PK3) and 7Z (PK7) archives for Doom.

Building
--------

### Windows

You will need to install CMake.  Download the binary distribution of CMake for Windows [here][1].  You will also need a C++ compiler that is capable of dealing with the C++11 used in the project.  I personally use Visual Studio 2013, which you can download [here][2], but if you prefer a GNU toolchain I suspect a MinGW distribution like TDM-GCC will work just as well, which you can grab [here][3].

Once you have installed CMake and a compiler, launch the CMake GUI.  Where it says **Where is the source code:**, point it at the wadmake checkout directory.  Where it says **Where to build the binaries:**, point it at a separate empty directory.  I recommend creating a `build` subdirectory in the wadmake checkout directory for this purpose, as `.gitignore` will automatically ignore this directory.

Click **Configure**.  From there, simply select your compiler from the dropdown
select-box and click **Finish**.  Once you do this, you will find a project file
or Makefile in the build directory.

[1]: http://www.cmake.org/download/
[2]: https://www.visualstudio.com/en-us/news/vs2013-community-vs.aspx
[3]: http://tdm-gcc.tdragon.net/

### Linux

You will need to install CMake.  If you want Readline support in `wadsh` you will also need to install the header files for Readline.  The following invocation ought to work on Debian/Ubuntu:

    aptitude install cmake libreadline-dev

The following invocation ought to work on RHEL/CentOS/Fedora:

    yum install cmake readline-devel

From there, clone the repository using git, change to the checkout directory, and run:

    mkdir build && cd build && cmake .. && make

`wadmake` and `wadsh` should be in the `build/bin` directory.  You can also run the included unit tests with `testwadmake` in the `build/test` directory.

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
