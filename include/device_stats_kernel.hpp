#ifndef ASTRO_ACCELERATE_DEVICE_STATS_KERNEL_HPP
#define ASTRO_ACCELERATE_DEVICE_STATS_KERNEL_HPP

void call_kernel_stats_kernel(const dim3 &block_size, const dim3 &grid_size, const int &smem_bytes, const cudaStream_t &stream,
			      const int &half_samps, float *const d_sum, float *const d_sum_square, float *const d_signal_power);

#endif
