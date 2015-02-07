enable_testing()

if(MSVC)
	set(TESTWADMAKE_EXE_CXXFLAGS "/W4")
else()
	set(TESTWADMAKE_EXE_CXXFLAGS "-Wall -Wextra -Wno-unknown-pragmas -std=c++11")
endif()

include_directories("${CMAKE_SOURCE_DIR}/src"
                    "${CMAKE_SOURCE_DIR}/lib/lua-5.3.0/src")

add_executable(testwadmake_exe testwadmake.cc)
set_target_properties(testwadmake_exe PROPERTIES COMPILE_FLAGS "${TESTWADMAKE_EXE_CXXFLAGS}")
set_target_properties(testwadmake_exe PROPERTIES OUTPUT_NAME testwadmake)
target_link_libraries(testwadmake_exe wadmake)

# Files needed for unit testing
file(DOWNLOAD "http://static.best-ever.org/wads/moo2d.wad" "${CMAKE_CURRENT_BINARY_DIR}/moo2d.wad"
     EXPECTED_MD5 "2e4635df68da25f78fde58ab179b8c2c" SHOW_PROGRESS)

file(DOWNLOAD "http://static.best-ever.org/wads/duel32f.pk3" "${CMAKE_CURRENT_BINARY_DIR}/duel32f.pk3"
     EXPECTED_MD5 "15dadb7e1976d2c7825e3221c4b649eb" SHOW_PROGRESS)
