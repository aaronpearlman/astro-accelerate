#ifndef ASTRO_ACCELERATE_DEVICE_SET_STRETCH_HPP
#define ASTRO_ACCELERATE_DEVICE_SET_STRETCH_HPP

extern void set_stretch_gpu(cudaEvent_t event, cudaStream_t stream, int samps, float mean, float *d_input);

#endif

