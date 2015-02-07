add_subdirectory("lua-5.3.0")

set(SKIP_INSTALL_ALL TRUE) # Nix zlib install targets
add_subdirectory("zlib-1.2.8")
