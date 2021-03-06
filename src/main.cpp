#include "headers_mains.hpp"
#include "host_main_function.hpp" // Added by Nassim.O
#include "host_debug.hpp"
#include "host_get_user_input.hpp"
#include "host_get_file_data.hpp"
#include "host_allocate_memory.hpp"
#include "host_get_recorded_data.hpp"
#include "host_info.hpp"
#include "params.hpp"

int main(int argc, char* argv[])
{
	// Internal code variables
	// File pointers
	FILE *fp = NULL;
	// Counters and flags
	int i = 0;
	int t = 0;
	int dm_range = 0;
	int range = 0;
	int nb_selected_dm = 0;
	int enable_debug = 0;
	int enable_analysis = 0;
	int enable_acceleration = 0;
	int enable_periodicity = 0;
	int output_dmt = 0;
	int enable_zero_dm = 0;
	int enable_zero_dm_with_outliers = 0;
	int enable_rfi = 0;
	int enable_old_rfi = 0;
	int enable_sps_baselinenoise=0;
	int enable_fdas_custom_fft = 0;
	int enable_fdas_inbin = 0;
	int enable_fdas_norm = 0;
	int enable_output_ffdot_plan = 0;
	int enable_output_fdas_list = 0;
	int analysis_debug = 0;
    int *inBin = NULL;
	int *outBin = NULL;
	int *ndms = NULL;
	int maxshift = 0;
	int max_ndms = 0;
	int max_samps = 0;
	int num_tchunks = 0;
	int total_ndms = 0;
	int multi_file = 1;
	float max_dm = 0.0f;
	int candidate_algorithm=0;
	int failsafe = 0;
	// Memory sizes and pointers
	size_t inputsize = 0;
	size_t outputsize = 0;
	size_t gpu_inputsize = 0;
	size_t gpu_outputsize = 0;
	size_t gpu_memory = 0;
	unsigned short *input_buffer = NULL;
	float ***output_buffer = NULL;
	unsigned short *d_input = NULL;
	float *d_output = NULL;
	float *dmshifts = NULL;
	float *user_dm_low = NULL;
	float *user_dm_high = NULL;
	float *user_dm_step = NULL;
	float *dm_low = NULL;
	float *dm_high = NULL;
	float *dm_step = NULL;
	float *selected_dm_low = NULL;
	float *selected_dm_high = NULL;
	// host info; memory available
       	size_t host_memory;
	// Telescope parameters
	int nchans = 0;
	int nsamp = 0;
	int nbits = 0;
	int nsamples = 0;
	int nifs = 0;
	int** t_processed = NULL;
	int nboots = -1;
	int ntrial_bins;
	int navdms = 1;
	int nsearch = 3;
	float aggression = 2.5;
	float narrow = 0.001f;
	float wide = 0.1f;
	int maxshift_original = 0;
	double tsamp_original = 0;
	long int inc = 0;
	float tstart = 0.0f;
	float tstart_local = 0.0f;
	float tsamp = 0.0f;
	float fch1 = 0.0f;
	float foff = 0.0f;
	// Analysis variables
	float power = 2.0f;
	float sigma_cutoff = 6.0f;
	float sigma_constant = 4.0f;
	float max_boxcar_width_in_sec = 0.5f;
	// Periodicity search
	float periodicity_sigma_cutoff = 6;
	int periodicity_nHarmonics = 32;

	// Timing parameters
	clock_t start_time = clock();

	// Users desired de-dispersion strategy. Pick up user defined values from the CLI.
	get_user_input(&fp, argc, argv, &multi_file, &enable_debug, &enable_analysis,
	    &enable_periodicity, &enable_acceleration, &enable_output_ffdot_plan,
	    &enable_output_fdas_list, &output_dmt, &enable_zero_dm,
	    &enable_zero_dm_with_outliers, &enable_rfi, &enable_old_rfi, &enable_fdas_custom_fft,
	    &enable_fdas_inbin, &enable_fdas_norm, &nboots, &ntrial_bins, &navdms,
	    &narrow, &wide, &aggression, &nsearch, &inBin, &outBin, &power, &sigma_cutoff,
	    &sigma_constant, &max_boxcar_width_in_sec, &range, &user_dm_low, &user_dm_high,
	    &user_dm_step, &candidate_algorithm, &enable_sps_baselinenoise, &selected_dm_low, &selected_dm_high, &nb_selected_dm, &analysis_debug, &failsafe, &periodicity_sigma_cutoff, &periodicity_nHarmonics);
	if (enable_debug == 1)
		debug(1, start_time, range, outBin, enable_debug, enable_analysis,
		output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low,
		user_dm_high, user_dm_step, dm_low, dm_high, dm_step, ndms, nchans,
		nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm,
		nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);
		
	// Reads telescope parameters from the header of the input file and then counts the number of samples in the input data file.
	get_file_data(&fp, &nchans, &nsamples, &nsamp, &nifs, &nbits, &tsamp, &tstart,
	    &fch1, &foff);
	if (enable_debug == 1)
		debug(3, start_time, range, outBin, enable_debug, enable_analysis,
		output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low,
		user_dm_high, user_dm_step, dm_low, dm_high, dm_step, ndms, nchans,
		nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm,
		nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	// get basic information of the system; available memory, etc.
	GetRamInKB(&host_memory);
//	printf("\nMemory available: %zu", host_memory/1024/1024);

	// Allocate memory on host.
	allocate_memory_cpu_input(&fp, gpu_memory, &host_memory, maxshift, num_tchunks, max_ndms,
				  total_ndms, nsamp, nchans, nbits, range, ndms, t_processed, &input_buffer,
	  &output_buffer, &d_input, &d_output, &gpu_inputsize, &gpu_outputsize,
	  &inputsize, &outputsize);

	if (enable_debug == 1)
		debug(5, start_time, range, outBin, enable_debug, enable_analysis,
		output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low,
		user_dm_high, user_dm_step, dm_low, dm_high, dm_step, ndms, nchans,
		nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm,
		nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	// Store the recorded telescope data contained in the input filterbank file in the allocated memory.
	get_recorded_data(&fp, nsamp, nchans, nbits, &input_buffer, &inputsize);
	if (enable_debug == 1)
		debug(7, start_time, range, outBin, enable_debug, enable_analysis,
		output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low,
		user_dm_high, user_dm_step, dm_low, dm_high, dm_step, ndms, nchans,
		nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm,
		nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	main_function
	(
	  argc, argv,
	  // Internal code variables
	  // File pointers
	  fp,
	  // Counters and flags
	  i, t, dm_range, range, enable_debug, enable_analysis, enable_acceleration, enable_output_ffdot_plan,
	  enable_output_fdas_list, enable_periodicity, output_dmt, enable_zero_dm, enable_zero_dm_with_outliers,
	  enable_rfi, enable_old_rfi, enable_sps_baselinenoise, enable_fdas_custom_fft, enable_fdas_inbin, enable_fdas_norm, inBin,
	  outBin, ndms, maxshift, max_ndms, max_samps, num_tchunks, total_ndms, multi_file, max_dm,
	  // Memory sizes and pointers
	  inputsize, outputsize, gpu_inputsize, gpu_outputsize, gpu_memory, host_memory,
	  input_buffer, output_buffer, d_input, d_output, dmshifts, user_dm_low,
	  user_dm_high, user_dm_step, dm_low, dm_high, dm_step,
	  // Telescope parameters
	  nchans, nsamp, nbits, nsamples, nifs, t_processed,  nboots, ntrial_bins,
	  navdms, nsearch, aggression, narrow, wide, maxshift_original,
	  tsamp_original, inc, tstart, tstart_local, tsamp, fch1, foff,
	  // Analysis variables
	  power, sigma_cutoff, sigma_constant, max_boxcar_width_in_sec, start_time, candidate_algorithm,
	  nb_selected_dm, selected_dm_low, selected_dm_high, analysis_debug, failsafe,
	  // Periodicity search
	  periodicity_sigma_cutoff, periodicity_nHarmonics
	);

	// write output here, not in the library
	if(fp != 0 && fp != NULL) fclose(fp);
	
	if(output_buffer != 0 && output_buffer != NULL) free(output_buffer);
	free(t_processed);
	if(dm_low != 0 && dm_low != NULL) free(dm_low);
	if(dm_high != 0 && dm_high != NULL) free(dm_high);
	if(dm_step != 0 && dm_step != NULL) free(dm_step);
	if(dmshifts != 0 && dmshifts != NULL) free(dmshifts);
	if(user_dm_low != 0 && user_dm_low != NULL) free(user_dm_low);
	if(user_dm_high != 0 && user_dm_high != NULL) free(user_dm_high);
	if(user_dm_step != 0 && user_dm_step != NULL) free(user_dm_step);
	if(nb_selected_dm > 0) {
	  free(selected_dm_low);
	  free(selected_dm_high);
	}
	
	return 0;	
}
