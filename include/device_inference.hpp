#ifndef ASTRO_ACCELERATE_DEVICE_INFERENCE_HPP
#define ASTRO_ACCELERATE_DEVICE_INFERENCE_HPP

void gpu_blocked_bootstrap(float **d_idata, int dms_to_average, int num_els, int ndms, int num_bins, int num_boots, double *mean_boot_out, double *mean_data_out, double *sd_boot_out);

#endif

