set(BSTRING_SOURCES
    "bstring/bstraux.c"
    "bstring/bstrlib.c")
set(BSTRING_HEADERS
    "bstring/bstraux.h"
    "bstring/bstrlib.h")

include_directories("bstring")

add_library(bstring STATIC ${BSTRING_SOURCES} ${BSTRING_HEADERS})
