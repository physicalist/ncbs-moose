# Find Numpy 
add_definitions(-DPYMOOSE)
include_directories(../basecode ../msg)

FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(moosepython STATIC
    ${SRCS}
)
