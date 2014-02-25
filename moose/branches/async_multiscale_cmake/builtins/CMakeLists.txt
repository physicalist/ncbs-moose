file(GLOB files_SRC 
    "*.h"
    "*.cpp"
)
include_directories(../msg)
include_directories(../basecode)
include_directories(../external/muparser)
add_library(builtins ${files_SRC})
