# distutils: language = c++
# distutils: include_dirs = ., cymoose, ../cymoose/
# distutils: extra_compiler_args = -DCYTHON
# distutils: libraries = basecode

cimport Eref as _Eref
cdef extern from "../biophysics/Compartment.h" namespace "moose":
    cdef cppclass Compartment:
        Compartment()
        void vSetVm(const _Eref.Eref& e, double Vm )
        double vGetVm( const _Eref.Eref& e) const
        void vSetEm(const _Eref.Eref& e, double Em )
        double vGetEm( const _Eref.Eref& e) const
        void vSetCm( const _Eref.Eref& e, double Cm )
        double vGetCm( const _Eref.Eref& e) const
        void vSetRm( const _Eref.Eref& e, double Rm )
        double vGetRm( const _Eref.Eref& e) const
        void vSetRa( const _Eref.Eref& e, double Ra )
        double vGetRa( const _Eref.Eref& e) const
        void vSetIm( const _Eref.Eref& e, double Im )
        double vGetIm( const _Eref.Eref& e) const
        void vSetInject( const _Eref.Eref& e, double Inject )
        double vGetInject( const _Eref.Eref& e) const
        void vSetInitVm( const _Eref.Eref& e, double initVm )
        double vGetInitVm( const _Eref.Eref& e) const
        void vSetDiameter( const _Eref.Eref& e, double diameter )
        double vGetDiameter( const _Eref.Eref& e) const
        void vSetLength( const _Eref.Eref& e, double length )
        double vGetLength(const _Eref.Eref& e, const _Eref.Eref& e) const
        void vSetX0(const _Eref.Eref& e, double value )
        double vGetX0( const _Eref.Eref& e) const
        void vSetY0(const _Eref.Eref& e, double value )
        double vGetY0( const _Eref.Eref& e) const
        void vSetZ0(const _Eref.Eref& e, double value )
        double vGetZ0( const _Eref.Eref& e) const
        void vSetX(const _Eref.Eref& e, double value )
        double vGetX( const _Eref.Eref& e) const
        void vSetY(const _Eref.Eref& e, double value )
        double vGetY( const _Eref.Eref& e) const
        void vSetZ(const _Eref.Eref& e, double value )
        double vGetZ( const _Eref.Eref& e) const
