file(GLOB biophysics_SRC 
    "*.h"
    "*.cpp"
    )
include_directories(../basecode)
add_library(biophysics ${biophysics_SRC})
