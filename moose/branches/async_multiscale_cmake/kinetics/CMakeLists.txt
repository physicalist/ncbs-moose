file(GLOB files_SRC 
    "*.h"
    "*.cpp"
)
include_directories(../msg)
include_directories(../basecode ../utility)
add_library(kinetics ${files_SRC})
target_link_libraries(kinetics basecode)
