file(GLOB random_SRC 
    "*.cpp"
)
include_directories(../basecode ..)
add_library(randnum ${random_SRC})
