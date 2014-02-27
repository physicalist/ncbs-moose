# distutils: language = c++
# distutils: include_dirs = ., cymoose, ../cymoose/
# distutils: extra_compiler_args = -DCYTHON
# distutils: libraries = basecode

cdef extern from "../biophysics/Compartment.h" namespace "moose":
    cdef cppclass Compartment:
        Compartment()
        void setVm( double Vm )
        double getVm() const
        void setEm( double Em )
        double getEm() const
        void setCm( double Cm )
        double getCm() const
        void setRm( double Rm )
        double getRm() const
        void setRa( double Ra )
        double getRa() const
        void setIm( double Im )
        double getIm() const
        void setInject( double Inject )
        double getInject() const
        void setInitVm( double initVm )
        double getInitVm() const
        void setDiameter( double diameter )
        double getDiameter() const
        void setLength( double length )
        double getLength() const
        void setX0( double value )
        double getX0() const
        void setY0( double value )
        double getY0() const
        void setZ0( double value )
        double getZ0() const
        void setX( double value )
        double getX() const
        void setY( double value )
        double getY() const
        void setZ( double value )
        double getZ() const
