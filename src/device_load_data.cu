//#include "headers/device_dedispersion_kernel.h"

//extern "C" void load_data(int i, float *device_pointer, float *host_pointer, size_t size, int nsamp, int maxshift, int nchans, int t_processed_s, int t_processed_c, float *dmshifts);

//{{{ load_data_from_host_to_device
#include "device_load_data.hpp"
#include "device_dedispersion_kernel.hpp"
#include "device_SPS_inplace_kernel.hpp"

void load_data(int i, int *inBin, unsigned short *device_pointer, unsigned short *host_pointer, int t_processed, int maxshift, int nchans, float *dmshifts)
{

	//{{{ Copy data and set up the GPU constants/variables.
	if (i == -1)
	{
		long int length = ( t_processed + maxshift );
		size_t size = nchans * length * sizeof(unsigned short);
		cudaMemcpy(device_pointer, host_pointer, size, cudaMemcpyHostToDevice);
		set_device_constants_dedispersion_kernel(nchans, length, t_processed, dmshifts);
	}
	else if (i > 0)
	{
	  long int length = ( t_processed + maxshift );
	  set_device_constants_dedispersion_kernel(length, t_processed);
	}
	//}}}

	float h_sqrt_taps[PD_MAXTAPS + 1];
	for (int f = 0; f <= PD_MAXTAPS; f++)
		h_sqrt_taps[f] = (float) sqrt((double) f);
	cudaMemcpyToSymbol(c_sqrt_taps, h_sqrt_taps, ( PD_MAXTAPS + 1 ) * sizeof(float));

}

//}}}
