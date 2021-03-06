#include "headers_mains.hpp"

#include <helper_cuda.h>

#include "device_bin.hpp"
#include "device_init.hpp"
#include "device_dedisperse.hpp"
#include "device_dedispersion_kernel.hpp"
#include "device_zero_dm.hpp"
#include "device_zero_dm_outliers.hpp"
#include "device_rfi.hpp"

// MSD
#include "device_MSD_Configuration.hpp"
#include "device_MSD.hpp"
#include "device_MSD_plane_profile.hpp"

#include "device_SPS_inplace_kernel.hpp" //Added by KA
#include "device_SPS_inplace.hpp" //Added by KA
#include "device_SNR_limited.hpp" //Added by KA
#include "device_SPS_long.hpp" //Added by KA
#include "device_threshold.hpp" //Added by KA
#include "device_single_FIR.hpp" //Added by KA
#include "device_analysis.hpp" //Added by KA
#include "device_periods.hpp" //Added by KA
#include "device_peak_find.hpp" //Added by KA
#include "device_power.hpp"
#include "device_harmonic_summing.hpp"



#include "device_load_data.hpp"
#include "device_corner_turn.hpp"
#include "device_save_data.hpp"
#include "host_acceleration.hpp"
#include "host_allocate_memory.hpp"
#include "host_analysis.hpp"
#include "host_export.hpp"
#include "host_periods.hpp"
#include "host_debug.hpp"
#include "host_get_file_data.hpp"
#include "host_get_recorded_data.hpp"
#include "host_get_user_input.hpp"
#include "host_help.hpp"
#include "host_rfi.hpp"
#include "host_stratagy.hpp"
#include "host_MSD_stratagy.hpp" //Adding for MSD permanent memory
#include "host_write_file.hpp"
#include "host_info.hpp"

// fdas
#include "device_acceleration_fdas.hpp"

#include "host_main_function.hpp"

#include "params.hpp"

#include "gpu_timer.hpp"


//#define EXPORT_DD_DATA

void main_function
	(
	int argc,
	char* argv[],
	// Internal code variables
	// File pointers
	FILE *fp,
	// Counters and flags
	int i,
	int t,
	int dm_range,
	int range,
	int enable_debug,
	int enable_analysis,
	int enable_acceleration,
	int enable_output_ffdot_plan,
	int enable_output_fdas_list,
	int enable_periodicity,
	int output_dmt,
	int enable_zero_dm,
	int enable_zero_dm_with_outliers,
	int enable_rfi,
	int enable_old_rfi,
	int enable_sps_baselinenoise,
	int enable_fdas_custom_fft,
	int enable_fdas_inbin,
	int enable_fdas_norm,
	int *inBin,
	int *outBin,
	int *ndms,
	int maxshift,
	int max_ndms,
	int max_samps,
	int num_tchunks,
	int total_ndms,
	int multi_file,
	float max_dm,
	// Memory sizes and pointers
  size_t inputsize,
  size_t outputsize,
	size_t gpu_inputsize,
	size_t gpu_outputsize,
	size_t gpu_memory,
	size_t host_memory,
  unsigned short  *input_buffer,
	float ***output_buffer,
	unsigned short  *d_input,
	float *d_output,
	float *dmshifts,
	float *user_dm_low,
	float *user_dm_high,
	float *user_dm_step,
	float *dm_low,
	float *dm_high,
	float *dm_step,
	// Telescope parameters
	int nchans,
	int nsamp,
	int nbits,
	int nsamples,
	int nifs,
	int **t_processed,
	int nboots,
	int ntrial_bins,
	int navdms,
	int nsearch,
	float aggression,
	float narrow,
	float wide,
	int	maxshift_original,
	double	tsamp_original,
	long int inc,
	float tstart,
	float tstart_local,
	float tsamp,
	float fch1,
	float foff,
	// Analysis variables
	float power,
	float sigma_cutoff,
	float sigma_constant,
	float max_boxcar_width_in_sec,
	clock_t start_time,
	int candidate_algorithm,
	int nb_selected_dm,
	float *selected_dm_low,
	float *selected_dm_high,
	int analysis_debug,
	int failsafe,
	float periodicity_sigma_cutoff,
	int periodicity_nHarmonics
	)
{

	// Initialise the GPU.	
	init_gpu(argc, argv, enable_debug, &gpu_memory);
	if(enable_debug == 1) debug(2, start_time, range, outBin, enable_debug, enable_analysis, output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low, user_dm_high,
	user_dm_step, dm_low, dm_high, dm_step, ndms, nchans, nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm, nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	checkCudaErrors(cudaGetLastError());
	
	// Calculate the dedispersion stratagy.
	stratagy(&maxshift, &max_samps, &num_tchunks, &max_ndms, &total_ndms, &max_dm, power, nchans, nsamp, fch1, foff, tsamp, range, user_dm_low, user_dm_high, user_dm_step,
                 &dm_low, &dm_high, &dm_step, &ndms, &dmshifts, inBin, &t_processed, &gpu_memory, enable_analysis);
	if(enable_debug == 1) debug(4, start_time, range, outBin, enable_debug, enable_analysis, output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low, user_dm_high,
	user_dm_step, dm_low, dm_high, dm_step, ndms, nchans, nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm, nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	checkCudaErrors(cudaGetLastError());

	// Allocate memory on host and device.
	allocate_memory_cpu_output(&fp, gpu_memory, &host_memory,  maxshift, num_tchunks, max_ndms, total_ndms, nsamp, nchans, nbits, range, ndms, t_processed, &input_buffer, &output_buffer, &d_input, &d_output, &gpu_inputsize, &gpu_outputsize, &inputsize, &outputsize);
	if(enable_debug == 1) debug(5, start_time, range, outBin, enable_debug, enable_analysis, output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low, user_dm_high,
	user_dm_step, dm_low, dm_high, dm_step, ndms, nchans, nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm, nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	checkCudaErrors(cudaGetLastError());
	
	// Allocate memory on host and device.
	allocate_memory_gpu(&fp, gpu_memory, maxshift, num_tchunks, max_ndms, total_ndms, nsamp, nchans, nbits, range, ndms, t_processed, &input_buffer, &output_buffer, &d_input, &d_output, &gpu_inputsize, &gpu_outputsize, &inputsize, &outputsize);
	if(enable_debug == 1) debug(5, start_time, range, outBin, enable_debug, enable_analysis, output_dmt, multi_file, sigma_cutoff, power, max_ndms, user_dm_low, user_dm_high,
	user_dm_step, dm_low, dm_high, dm_step, ndms, nchans, nsamples, nifs, nbits, tsamp, tstart, fch1, foff, maxshift, max_dm, nsamp, gpu_inputsize, gpu_outputsize, inputsize, outputsize);

	checkCudaErrors(cudaGetLastError());

	//compute the worst case for SPDT
	unsigned long int MSD_data_info;
	size_t MSD_profile_size_in_bytes;
	int h_MSD_DIT_width;
	//declare memory for SPDT
	float *d_MSD_workarea = NULL;
        float *d_MSD_interpolated = NULL;
        ushort *d_MSD_output_taps = NULL;
	//Run allocation only if needed
	if (enable_analysis == 1) {
		stratagy_MSD(max_ndms,max_boxcar_width_in_sec, tsamp, t_processed[0][0], &MSD_data_info, &MSD_profile_size_in_bytes, &h_MSD_DIT_width);
		allocate_memory_MSD(&d_MSD_workarea, &d_MSD_output_taps, &d_MSD_interpolated, MSD_data_info, h_MSD_DIT_width, t_processed[0][0], MSD_profile_size_in_bytes);
	}

	checkCudaErrors(cudaGetLastError());
	
	// Clip RFI
	if (enable_rfi) {
		printf("\nPerforming new CPU rfi...");
		rfi(nsamp, nchans, &input_buffer);
	}
	/*
	 FILE	*fp_o;

	 if ((fp_o=fopen("rfi_clipped.dat", "wb")) == NULL) {
	 fprintf(stderr, "Error opening output file!\n");
	 exit(0);
	 }
	 fwrite(input_buffer, nchans*nsamp*sizeof(unsigned short), 1, fp_o);
	 */

	printf("\nDe-dispersing...");
	GpuTimer timer;
	timer.Start();


	tsamp_original = tsamp;
	maxshift_original = maxshift;

	//float *out_tmp;
	//out_tmp = (float *) malloc(( t_processed[0][0] + maxshift ) * max_ndms * sizeof(float));
	//memset(out_tmp, 0.0f, t_processed[0][0] + maxshift * max_ndms * sizeof(float));

	for (t = 0; t < num_tchunks; t++) {
		printf("\nt_processed:\t%d, %d", t_processed[0][t], t);
		
		checkCudaErrors(cudaGetLastError());

		load_data(-1, inBin, d_input, &input_buffer[(long int) ( inc * nchans )], t_processed[0][t], maxshift, nchans, dmshifts);

		checkCudaErrors(cudaGetLastError());
		
		if (enable_zero_dm) {
			zero_dm(d_input, nchans, t_processed[0][t]+maxshift,nbits);
		}
		
		checkCudaErrors(cudaGetLastError());
		
		if (enable_zero_dm_with_outliers) {
			zero_dm_outliers(d_input, nchans, t_processed[0][t]+maxshift);
	 	}
		
		checkCudaErrors(cudaGetLastError());
	
		corner_turn(d_input, d_output, nchans, t_processed[0][t] + maxshift);
		
		checkCudaErrors(cudaGetLastError());
		
		if (enable_old_rfi) {
			printf("\nPerforming old GPU rfi...");
 			rfi_gpu(d_input, nchans, t_processed[0][t]+maxshift);
		}
		
		checkCudaErrors(cudaGetLastError());
		
		int oldBin = 1;
		for (dm_range = 0; dm_range < range; dm_range++) {
			printf("\n\n%f\t%f\t%f\t%d", dm_low[dm_range], dm_high[dm_range], dm_step[dm_range], ndms[dm_range]), fflush(stdout);
			printf("\nAmount of telescope time processed: %f", tstart_local);
			maxshift = maxshift_original / inBin[dm_range];

			checkCudaErrors(cudaGetLastError());
			
			cudaDeviceSynchronize();
			
			checkCudaErrors(cudaGetLastError());
			
			load_data(dm_range, inBin, d_input, &input_buffer[(long int) ( inc * nchans )], t_processed[dm_range][t], maxshift, nchans, dmshifts);
			
			checkCudaErrors(cudaGetLastError());
			
			if (inBin[dm_range] > oldBin) {
				bin_gpu(d_input, d_output, nchans, t_processed[dm_range - 1][t] + maxshift * inBin[dm_range]);
				( tsamp ) = ( tsamp ) * 2.0f;
			}
			
			checkCudaErrors(cudaGetLastError());
			
			dedisperse(dm_range, t_processed[dm_range][t], inBin, dmshifts, d_input, d_output, nchans, ( t_processed[dm_range][t] + maxshift ), maxshift, &tsamp, dm_low, dm_high, dm_step, ndms, nbits, failsafe);
		
			checkCudaErrors(cudaGetLastError());
			
			if ( (enable_acceleration == 1) || (enable_periodicity == 1) || (analysis_debug ==1) ) {
				// gpu_outputsize = ndms[dm_range] * ( t_processed[dm_range][t] ) * sizeof(float);
				//save_data(d_output, out_tmp, gpu_outputsize);

				//#pragma omp parallel for
				for (int k = 0; k < ndms[dm_range]; k++) {
					//memcpy(&output_buffer[dm_range][k][inc / inBin[dm_range]], &out_tmp[k * t_processed[dm_range][t]], sizeof(float) * t_processed[dm_range][t]);

					save_data_offset(d_output, k * t_processed[dm_range][t], output_buffer[dm_range][k], inc / inBin[dm_range], sizeof(float) * t_processed[dm_range][t]);
				}
			//	save_data(d_output, &output_buffer[dm_range][0][((long int)inc)/inBin[dm_range]], gpu_outputsize);
			}

			if (output_dmt == 1)
			{
				//for (int k = 0; k < ndms[dm_range]; k++)
				//	write_output(dm_range, t_processed[dm_range][t], ndms[dm_range], gpu_memory, output_buffer[dm_range][k], gpu_outputsize, dm_low, dm_high);
				//write_output(dm_range, t_processed[dm_range][t], ndms[dm_range], gpu_memory, out_tmp, gpu_outputsize, dm_low, dm_high);
			}
			
			checkCudaErrors(cudaGetLastError());
			
			if (enable_analysis == 1) {
				
				printf("\n VALUE OF ANALYSIS DEBUG IS %d\n", analysis_debug);

				if (analysis_debug == 1) {
					float *out_tmp;
					gpu_outputsize = ndms[dm_range] * ( t_processed[dm_range][t] ) * sizeof(float);
					out_tmp = (float *) malloc(( t_processed[0][0] + maxshift ) * max_ndms * sizeof(float));
					memset(out_tmp, 0.0f, t_processed[0][0] + maxshift * max_ndms * sizeof(float));
					save_data(d_output, out_tmp, gpu_outputsize);
					analysis_CPU(dm_range, tstart_local, t_processed[dm_range][t], (t_processed[dm_range][t]+maxshift), nchans, maxshift, max_ndms, ndms, outBin, sigma_cutoff, out_tmp,dm_low, dm_high, dm_step, tsamp, max_boxcar_width_in_sec);
					free(out_tmp);
				}
				else {
					unsigned int *h_peak_list_DM;
					unsigned int *h_peak_list_TS;
					float *h_peak_list_SNR;
					unsigned int *h_peak_list_BW;
					size_t max_peak_size;
					size_t peak_pos;
					max_peak_size = (size_t) ( ndms[dm_range]*t_processed[dm_range][t]/2 );
					h_peak_list_DM  = (unsigned int*) malloc(max_peak_size*sizeof(unsigned int));
					h_peak_list_TS  = (unsigned int*) malloc(max_peak_size*sizeof(unsigned int));
					h_peak_list_SNR = (float*) malloc(max_peak_size*sizeof(float));
					h_peak_list_BW  = (unsigned int*) malloc(max_peak_size*sizeof(unsigned int));

					peak_pos=0;
					analysis_GPU(h_peak_list_DM, h_peak_list_TS, h_peak_list_SNR, h_peak_list_BW, &peak_pos, max_peak_size, dm_range, tstart_local, t_processed[dm_range][t], inBin[dm_range], outBin[dm_range], &maxshift, max_ndms, ndms, sigma_cutoff, sigma_constant, max_boxcar_width_in_sec, d_output, dm_low, dm_high, dm_step, tsamp, candidate_algorithm, d_MSD_workarea, d_MSD_output_taps, d_MSD_interpolated, MSD_data_info, enable_sps_baselinenoise);

					free(h_peak_list_DM);
					free(h_peak_list_TS);
					free(h_peak_list_SNR);
					free(h_peak_list_BW);
				}

				// This is for testing purposes and should be removed or commented out
				//analysis_CPU(dm_range, tstart_local, t_processed[dm_range][t], (t_processed[dm_range][t]+maxshift), nchans, maxshift, max_ndms, ndms, outBin, sigma_cutoff, out_tmp,dm_low, dm_high, dm_step, tsamp);
			}
			oldBin = inBin[dm_range];
		}

		//memset(out_tmp, 0.0f, t_processed[0][0] + maxshift * max_ndms * sizeof(float));

		inc = inc + t_processed[0][t];
		printf("\nINC:\t%ld", inc);
		tstart_local = ( tsamp_original * inc );
		tsamp = tsamp_original;
		maxshift = maxshift_original;
	}

	timer.Stop();
	float time = timer.Elapsed() / 1000;

	printf("\n\n === OVERALL DEDISPERSION THROUGHPUT INCLUDING SYNCS AND DATA TRANSFERS ===\n");

	printf("\n(Performed Brute-Force Dedispersion: %g (GPU estimate)",  time);
	printf("\nAmount of telescope time processed: %f", tstart_local);
	printf("\nNumber of samples processed: %ld", inc);
	printf("\nReal-time speedup factor: %lf", ( tstart_local ) / time);

	cudaFree(d_input);
	cudaFree(d_output);
	// clean the SPDT memory only if
	if (d_MSD_interpolated != NULL) cudaFree(d_MSD_interpolated);
	if (d_MSD_workarea != NULL) cudaFree(d_MSD_workarea);
	if (d_MSD_output_taps != NULL) cudaFree(d_MSD_output_taps);
	//free(out_tmp);
	free(input_buffer);
	
	#ifdef EXPORT_DD_DATA
		size_t DMs_per_file;
		int *ranges_to_export;
		ranges_to_export = new int[range];
		for(int f=0; f<range; f++) ranges_to_export[f]=1;
		printf("\n\n");
		printf("Exporting dedispersion data...\n");
		DMs_per_file = Calculate_sd_per_file_from_file_size(1000, inc, 1);
		printf("  DM per file: %d;\n", DMs_per_file);
		Export_DD_data(range, output_buffer, inc, ndms, inBin, dm_low, dm_high, dm_step, "DD_data", ranges_to_export, DMs_per_file);
		delete[] ranges_to_export;
	#endif

	double time_processed = ( tstart_local ) / tsamp_original;
	double dm_t_processed = time_processed * total_ndms;
	double all_processed = dm_t_processed * nchans;
	printf("\nGops based on %.2lf ops per channel per tsamp: %f", NOPS, ( ( NOPS * all_processed ) / ( time ) ) / 1000000000.0);
	int num_reg = SNUMREG;
	float num_threads = total_ndms * ( t_processed[0][0] ) / ( num_reg );
	float data_size_loaded = ( num_threads * nchans * sizeof(ushort) ) / 1000000000;
	float time_in_sec = time;
	float bandwidth = data_size_loaded / time_in_sec;
	printf("\nDevice global memory bandwidth in GB/s: %f", bandwidth);
	printf("\nDevice shared memory bandwidth in GB/s: %f", bandwidth * ( num_reg ));
	float size_gb = ( nchans * ( t_processed[0][0] ) * sizeof(float) * 8 ) / 1000000000.0;
	printf("\nTelescope data throughput in Gb/s: %f", size_gb / time_in_sec);

	if (enable_periodicity == 1) {
		//
		GpuTimer timer;
		timer.Start();
		//
		GPU_periodicity(range, nsamp, max_ndms, inc, periodicity_sigma_cutoff, output_buffer, ndms, inBin, dm_low, dm_high, dm_step, tsamp_original, periodicity_nHarmonics, candidate_algorithm, enable_sps_baselinenoise, sigma_constant);
		//
		timer.Stop();
		float time = timer.Elapsed()/1000;
		printf("\n\n === OVERALL PERIODICITY THROUGHPUT INCLUDING SYNCS AND DATA TRANSFERS ===\n");

		printf("\nPerformed Peroidicity Location: %f (GPU estimate)", time);
		printf("\nAmount of telescope time processed: %f", tstart_local);
		printf("\nNumber of samples processed: %ld", inc);
		printf("\nReal-time speedup factor: %f", ( tstart_local ) / ( time ));
	}

	if (enable_acceleration == 1) {
		// Input needed for fdas is output_buffer which is DDPlan
		// Assumption: gpu memory is free and available
		//
		GpuTimer timer;
		timer.Start();
		// acceleration(range, nsamp, max_ndms, inc, nboots, ntrial_bins, navdms, narrow, wide, nsearch, aggression, sigma_cutoff, output_buffer, ndms, inBin, dm_low, dm_high, dm_step, tsamp_original);
		acceleration_fdas(range, nsamp, max_ndms, inc, nboots, ntrial_bins, navdms, narrow, wide, nsearch, aggression, sigma_cutoff,
						  output_buffer, ndms, inBin, dm_low, dm_high, dm_step, tsamp_original, enable_fdas_custom_fft, enable_fdas_inbin, enable_fdas_norm, sigma_constant, enable_output_ffdot_plan, enable_output_fdas_list);
		//
		timer.Stop();
		float time = timer.Elapsed()/1000;
		printf("\n\n === OVERALL TDAS THROUGHPUT INCLUDING SYNCS AND DATA TRANSFERS ===\n");

		printf("\nPerformed Acceleration Location: %lf (GPU estimate)", time);
		printf("\nAmount of telescope time processed: %f", tstart_local);
		printf("\nNumber of samples processed: %ld", inc);
		printf("\nReal-time speedup factor: %lf", ( tstart_local ) / ( time ));
	}
}
