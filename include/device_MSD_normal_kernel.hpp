#ifndef ASTRO_ACCELERATE_DEVICE_MSD_NORMAL_KERNEL_HPP
#define ASTRO_ACCELERATE_DEVICE_MSD_NORMAL_KERNEL_HPP

void call_kernel_MSD_GPU_limited(const dim3 &grid_size, const dim3 &block_size, float const *const d_input, float *const d_output, const int &y_steps, const int &nTimesamples, const int &offset);

#endif
