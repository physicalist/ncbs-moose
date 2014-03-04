cmake_minimum_required(VERSION 2.8)
include_directories(../basecode ../utility ../kinetics)
add_library(ksolve
	KinSparseMatrix.cpp	
	ZombiePool.cpp 
	ZombieBufPool.cpp 
	ZombieFuncPool.cpp 
	ZombieReac.cpp 
	ZombieEnz.cpp 
	ZombieMMenz.cpp 
	VoxelPools.cpp 
	RateTerm.cpp 
	Stoich.cpp 
	Ksolve.cpp 
    )

target_link_libraries(ksolve basecode)
