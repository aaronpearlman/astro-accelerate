#ifndef ASTRO_ACCELERATE_DEVICE_SPS_LONG_KERNEL_HPP
#define ASTRO_ACCELERATE_DEVICE_SPS_LONG_KERNEL_HPP

#include <cuda.h>
#include <cuda_runtime.h>

#include "params.hpp"
#include "device_SPS_inplace_kernel.hpp"

void call_kernel_SPDT_GPU_1st_plane(const dim3 &grid_size, const dim3 &block_size, float const *const d_input,
				    float *const d_bv_out, float *const d_decimated,
				    float *const d_output_SNR, ushort *const d_output_taps, float2 const *const d_MSD,
				    const int &nTimesamples, const int &nBoxcars, const int &dtm);

void call_kernel_SPDT_GPU_Nth_plane(const dim3 &grid_size, const dim3 &block_size, float const *const d_input,
				    float *const d_bv_in, float *const d_bv_out, float *const d_decimated,
				    float *const d_output_SNR, ushort *const d_output_taps, float2 const *const d_MSD,
				    const int &nTimesamples, const int &nBoxcars, const int &startTaps,
				    const int &DIT_value, const int &dtm);

#endif

