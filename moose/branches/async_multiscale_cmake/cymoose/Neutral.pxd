# distutils: language = c++
# distutils: include_dirs = ., cymoose
# distutils: extra_compiler_args = -DCYTHON

cdef extern from "../shell/Neutral.h":
    cdef cppclass Neutral:
        pass

