find_package(Sphinx)

if(SPHINX_EXECUTABLE)
	set(SPHINX_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")
	set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")
	set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
	               "${SPHINX_BUILD_DIR}/conf.py" @ONLY)

	add_custom_target(docs
	                  "${SPHINX_EXECUTABLE}" -q -b html
	                  -c "${SPHINX_BUILD_DIR}"
	                  -d "${SPHINX_CACHE_DIR}"
	                  "${CMAKE_CURRENT_SOURCE_DIR}"
	                  "${SPHINX_HTML_DIR}"
	                  COMMENT "Building documentation")
else()
	message(WARNING "Sphinx not found: docs target will not be built")
endif()
