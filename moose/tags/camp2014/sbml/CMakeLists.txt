file(GLOB files_SRC 
    "*.h"
    "*.cpp"
)
include_directories(../msg)
include_directories(../basecode)
add_library(moose_sbml ${files_SRC})
target_link_libraries(moose_sbml ${SBML_LIBRARIES})
