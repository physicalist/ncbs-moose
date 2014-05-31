///////////////////////////////////////////////////////////
//  Field.h
//
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A72EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
#define A72EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_

#include "../../headers.h"
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/fill.h>

namespace pn2s
{
namespace solvers
{


template <typename T, int arch>
class Field
{
private:
	size_t _size;
public:
	enum FieldType {TYPE_IO, TYPE_INPUT, TYPE_OUTPUT} fieldType;
	T* host;
	int host_inc;
	T* device;
	int device_inc;


	Field();
	//TODO: Use it!
	Field(FieldType t);

	virtual ~Field();

	Error_PN2S AllocateMemory(size_t size);
//	Error_PN2S AllocateMemory(size_t size, int inc);
	Error_PN2S Host2Device_Sync();
	Error_PN2S Device2Host_Sync();

	Error_PN2S Send2Device(Field& _hostResource);
	Error_PN2S Send2Host(Field& _hostResource);

	thrust::device_ptr<T> DeviceStart() {return thrust::device_ptr<T> (device); }
	thrust::device_ptr<T> DeviceEnd() {return thrust::device_ptr<T> (device+_size); }

	T operator [](int i) const {return host[i];}
	T & operator [](int i) {return host[i];}
private:

};


}
}

#endif // !defined(A72EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
