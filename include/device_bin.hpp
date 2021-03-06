#ifndef ASTRO_ACCELERATE_DEVICE_BIN_HPP
#define ASTRO_ACCELERATE_DEVICE_BIN_HPP

#include <time.h>
#include <stdio.h>
#include "params.hpp"
#include "device_corner_turn_kernel.hpp"
#include "gpu_timer.hpp"

#include "device_binning_kernel.hpp"

extern void bin_gpu(unsigned short *d_input, float *d_output, int nchans, int nsamp);
extern int GPU_DiT_v2_wrapper(float *d_input, float *d_output, int nDMs, int nTimesamples);

#endif

