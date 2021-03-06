#ifndef ASTRO_ACCELERATE_DEVICE_MSD_SHARED_KERNEL_FUNCTIONS_HPP
#define ASTRO_ACCELERATE_DEVICE_MSD_SHARED_KERNEL_FUNCTIONS_HPP

void call_kernel_MSD_GPU_final_regular(const dim3 &grid_size, const dim3 &block_size, float *const d_input, float *const d_output, const int &size);
void call_kernel_MSD_GPU_final_regular(const dim3 &grid_size, const dim3 &block_size, float *const d_input, float *const d_MSD, float *const d_pp, const int &size);
void call_kernel_MSD_GPU_final_nonregular(const dim3 &grid_size, const dim3 &block_size, float *const d_input, float *const d_MSD, const int &size);
void call_kernel_MSD_GPU_final_nonregular(const dim3 &grid_size, const dim3 &block_size, float *const d_input, float *const d_MSD, float *const d_pp, const int &size);
void call_kernel_MSD_GPU_Interpolate_linear(const dim3 &grid_size, const dim3 &block_size, float *const d_MSD_DIT, float *const d_MSD_interpolated, int *const d_MSD_DIT_widths, const int &MSD_DIT_size, int *const boxcar, const int &max_width_performed);
#endif
