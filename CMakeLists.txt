# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0 FATAL_ERROR)
set(PROJECT_NAME_STR astro-accelerate)
set(PROJECT_LIB_NAME ASTRO_ACCELERATE)
PROJECT(${PROJECT_NAME_STR} LANGUAGES CXX CUDA)

find_package(OpenMP)
if(OPENMP_FOUND)
    message("-- INFO: OpenMP FLAGS: ${OpenMP_CXX_FLAGS}")
endif()

find_package(CUDA REQUIRED)

# Set NVCC flags
set(PROJECT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CUDA_LINK_LIBRARIES_KEYWORD PUBLIC)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS})

list(APPEND CUDA_NVCC_FLAGS --ptxas-options= -Xptxas -dlcm=cg -use_fast_math)
list(APPEND CUDA_NVCC_FLAGS -g;)
list(APPEND CUDA_NVCC_FLAGS -Xptxas -O3;)
list(APPEND CUDA_NVCC_FLAGS -lineinfo;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-O3;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-lm;)
list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-Wall;)

if(OPENMP_FOUND)
	list(APPEND CUDA_NVCC_FLAGS -Xcompiler;-fopenmp;)
	list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -O3 -fopenmp -lm -Wall")
else()
	list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -O3 -lm -Wall")
endif()

if(NOT DEFINED CUDA_ARCH)  
	set(CUDA_ARCH "ALL")
	message("-- INFO: Setting CUDA_ARCH to ALL.")
	message("-- INFO: The target CUDA architecture can be specified using:")
	message("-- INFO:   -DCUDA_ARCH=\"<arch>\"")
	message("-- INFO: where <arch> is one or more of:")
	message("-- INFO:   3.5, 3.7, 5.0, 5.2, 6.0, 6.1, 6.2, 7.0 or ALL.")
	message("-- INFO: Separate multiple architectures with semicolons.")
endif()

foreach(ARCH ${CUDA_ARCH})
	if(ARCH MATCHES ALL|[Aa]ll)
		message("-- INFO: Building CUDA device code for Kepler,")
		message("-- INFO: Maxwell and Pascal architectures")
	   	list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_35,code=sm_35)
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_37,code=sm_37)
	   	list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_50,code=sm_50)
	   	list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_52,code=sm_52)
	   	list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_60,code=sm_60)
	   	list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_61,code=sm_61)
	   	list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_62,code=sm_62)
	elseif(ARCH MATCHES 3.5)
		message("-- INFO: Building CUDA device code for architecture 3.5")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_35,code=sm_35)
	elseif(ARCH MATCHES 3.7)
		message("-- INFO: Building CUDA device code for architecture 3.7")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_37,code=sm_37)
	elseif(ARCH MATCHES 5.0)
		message("-- INFO: Building CUDA device code for architecture 5.0")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_50,code=sm_50)
	elseif(ARCH MATCHES 5.2)
		message("-- INFO: Building CUDA device code for architecture 5.2")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_52,code=sm_52)
	elseif(ARCH MATCHES 6.0)
		message("-- INFO: Building CUDA device code for architecture 6.0")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_60,code=sm_60)
	elseif(ARCH MATCHES 6.1)
		message("-- INFO: Building CUDA device code for architecture 6.1")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_61,code=sm_61)
	elseif(ARCH MATCHES 6.2)
		message("-- INFO: Building CUDA device code for architecture 6.2")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_62,code=sm_62)
	elseif(ARCH MATCHES 7.0)
		message("-- INFO: Building CUDA device code for architecture 7.0")
		list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_70,code=sm_70)
	elseif(ARCH MATCHES 7.5)
	        message("-- INFO: Building CUDA device code for architecture 7.5")
	        list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_75,code=sm_75)	
	else()
	message(FATAL_ERROR "-- CUDA_ARCH ${ARCH} not recognised or not defined")
	endif()
endforeach()

# Status information
message(STATUS "Using: ${CMAKE_CXX_COMPILER} with compiler ID ${CMAKE_CXX_COMPILER_ID} and compiler version ${CXX_VERSION}")
message(STATUS "Using CXX compilation flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Using CUDA NVCC flags ${CUDA_NVCC_FLAGS}")

# Include and linker directories
link_directories(${CUDA_LIBRARY_DIRS})
include_directories(include/)
include_directories(${PROJECT_BASE_DIR})
include_directories($ENV{CUDA_INSTALL_PATH}/include/)
include_directories($ENV{CUDA_INSTALL_PATH}/samples/common/inc/)
include_directories(${CUDA_LIBRARY_DIRS})

# CUDA library object
file(GLOB_RECURSE GPU_SOURCE_FILES "src/*.cu" "src/*.cpp")
cuda_add_library(astroaccelerate SHARED ${GPU_SOURCE_FILES})
target_link_libraries(astroaccelerate PRIVATE ${CUDA_LIBRARY_DIRS} ${CUDA_CUFFT_LIBRARIES} ${CUDA_curand_LIBRARY})
set_target_properties(astroaccelerate PROPERTIES CUDA_SEPARABLE_COMPILATION OFF)
set_target_properties(astroaccelerate PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(astroaccelerate PROPERTIES BUILD_SHARED_LIBS ON)

# Standalone executable to link against CUDA library
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} astroaccelerate)
