file(GLOB files_SRC 
    "*.h"
    "*.cpp"
)
include_directories(../msg)
include_directories(../basecode)
include_directories(../utility)
add_library(mesh ${files_SRC})
