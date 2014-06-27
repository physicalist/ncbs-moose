CU_SRCS += \
	GpuInterface.cu \
	GpuKernels.cu \

CU_DEPS += \
	CudaModule.d \

OBJS += \
	GpuInterface.o \
	GpuKernels.o \

NVCC_FLAGS = --compile -G -g -O0 -Xcompiler -fPIC -x cu -arch=sm_20

ifeq ($(BUILD),debug)
NVCC_FLAGS += -DDO_UNIT_TESTS
CU_SRCS += testGpuKernels.cu
OBJS += testGpuKernels.o
endif

TARGET_FLAGS = -shared -link

# Preprocessing is already over. This should not be needed.
#ifeq ($(BUILD),debug)
#TARGET_FLAGS += -DDO_UNIT_TESTS
#endif

all: libmooseCudaLibrary.so

libmooseCudaLibrary.so: $(OBJS) $(HEADERS)
	nvcc $(TARGET_FLAGS) -o "libmooseCudaLibrary.so" $(OBJS)

HSOLVE_HEADERS = \
	../../basecode/header.h \
	../HSolveStruct.h \
	../RateLookup.h \
	../HinesMatrix.h \
	../HSolvePassive.h \
	../HSolveActive.h \
	../HSolve.h \

GpuInterface.o: GpuInterface.cu GpuInterface.h GpuKernels.h $(HSOLVE_HEADERS)
	nvcc $(NVCC_FLAGS) -o GpuInterface.o GpuInterface.cu

GpuKernels.o: GpuKernels.cu GpuKernels.h ../HSolveStruct.h ../HinesMatrix.h
	nvcc $(NVCC_FLAGS) -o GpuKernels.o GpuKernels.cu

testGpuKernels.o: testGpuKernels.h testGpuKernels.cu $(HSOLVE_HEADERS)
	nvcc $(NVCC_FLAGS) -o testGpuKernels.o testGpuKernels.cu

#%.o: %.cu
	#nvcc -G -g -O0 -Xcompiler -fPIC -odir "" -M -o "$(@:%.o=%.d)" "$<"
	#nvcc --compile -G -O0 -Xcompiler -fPIC -g -x cu -o  "$@" "$<"

clean:
	rm -rf $(CU_DEPS) $(OBJS) libmooseCudaLibrary.so

#test: testGpuKernels.cu libmooseCudaLibrary.so
#	LD_LIBRARY_PATH="$$LD_LIBRARY_PATH:./" nvcc -g -G -lcuda -lcudart -L./ \
#		-lmooseCudaLibrary -o testGpuKernels testGpuKernels.cu -arch=sm_20
