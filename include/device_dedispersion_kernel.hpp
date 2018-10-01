#ifndef ASTRO_ACCELERATE_DEDISPERSION_KERNEL_HPP
#define ASTRO_ACCELERATE_DEDISPERSION_KERNEL_HPP

#include "params.hpp"

//These device variables and definitions are needed by device_dedispersion_kernel.cu and device_load_data.cu
// Stores temporary shift values
#define ARRAYSIZE SDIVINT * SDIVINDM
__device__  __shared__ ushort2 f_line[UNROLLS][ARRAYSIZE + 2];

void set_device_constants_dedispersion_kernel(const int& nchans, const int& length, const int& t_processed, const float *dm_shifts);
void set_device_constants_dedispersion_kernel(const long int& length, const int& t_processed);
void call_kernel_shared_dedisperse_kernel(dim3 block_size, dim3 grid_size, int bin, unsigned short *d_input, float *d_output, float mstartdm, float mdmstep);
void call_kernel_shared_dedisperse_kernel_16(dim3 block_size, dim3 grid_size, int bin, unsigned short *d_input, float *d_output, float mstartdm, float mdmstep);
void call_kernel_cache_dedisperse_kernel(dim3 block_size, dim3 grid_size, int bin, unsigned short *d_input, float *d_output, float mstartdm, float mdmstep);

#endif
