#ifndef ASTRO_ACCELERATE_AA_CONFIG_HPP
#define ASTRO_ACCELERATE_AA_CONFIG_HPP

#include <string>
#include <vector>
#include <wordexp.h>
#include "aa_pipeline.hpp"

struct DM {
  float low;
  float high;
};

struct aa_config_flags {
  int multi_file;
  int enable_debug;
  int enable_analysis;
  int enable_periodicity;
  int enable_acceleration;
  int enable_output_ffdot_plan;
  int enable_output_fdas_list;
  int output_dmt;
  int enable_zero_dm;
  int enable_zero_dm_with_outliers;
  int enable_rfi;
  int enable_old_rfi;
  int enable_fdas_custom_fft;
  int enable_fdas_inbin;
  int enable_fdas_norm;
  int nboots;
  int ntrial_bins;
  int navdms;
  float narrow;
  float wide;
  float aggression;
  float nsearch;
  std::vector<int> inBin;
  std::vector<int> outBin;
  float power;
  float sigma_cutoff;
  float sigma_constant;
  float max_boxcar_width_in_sec;
  int range;
  float user_dm_low;
  float user_dm_high;
  float user_dm_steep;
  int candidate_algorithm;
  int enable_sps_baselinenoise;
  std::vector<DM> selected_dm;
  struct User_DM {
    float low;
    float high;
    float step;
  };
  std::vector<User_DM> user_dm;
  
  int nb_selected_dm;
  int analysis_debug;
  int failsafe;
  int periodicity_sigma_cutoff;
  int periodicity_nHarmonics;
};

class aa_config {
public:
  aa_config(const int argc, char *const argv[], const std::string &path, aa_pipeline &pipeline) {
    fpath = path;
    get_user_input(argc, argv);
  }

  aa_config(const aa_config_flags &flags) {
    flg = flags;
  }

  ~aa_config() {

  }

protected:
  std::string fpath;
  aa_config_flags flg;  // configuration flags
  bool get_user_input(int argc, char *const argv[]) {
    FILE *fp_in = NULL;   // Path of the input file (configuration)
    FILE *fp = NULL;      // Path to the input data file (fil file)
    
    char string[100];
    
    if (argc < 2)
      {
	fprintf(stderr, "Need input file.\n");
	return false;
      }
    else if (argc == 2 && strcmp(argv[1], "-help") != 0)
      {
	if (( fp_in = fopen(argv[1], "r") ) == NULL)
	  {
	    fprintf(stderr, "Invalid input file!\n");
	    return false;
	  }
	flg.range = 0;
	while (!feof(fp_in))
	  {
	    if ( fscanf(fp_in, "%s", string) == 0 )
	      {
		fprintf(stderr, "failed to read input\n");
		return false;
	      }
	    if (strcmp(string, "range") == 0) ++flg.range;
	    if (strcmp(string, "selected_dm") == 0) ++flg.nb_selected_dm;
	  }
	rewind(fp_in);

	flg.user_dm.resize(flg.range);
	flg.outBin.resize(flg.range);
	flg.inBin.resize(flg.range);


	// temporary variables to read dm range
	float temp_low  = 0;
	float temp_high = 0;
	float temp_step = 0;
	int temp_in_bin = 0;
	int temp_out_bin= 0;

	// read dm range if enabled
	int i=0;
	while (!feof(fp_in))
	  {
	    if ( fscanf(fp_in, "%s %f %f %f %d %d\n", string, &temp_low, &temp_high, &temp_step, &temp_in_bin, &temp_out_bin) == 0 )
	      {
		fprintf(stderr, "failed to read input\n");
		exit(0);
	      }
	    if (strcmp(string, "range") == 0)
	      {
		flg.user_dm[i] = {temp_low, temp_high, temp_step};
		flg.inBin[i]  = temp_in_bin;
		flg.outBin[i] = temp_out_bin;
		++i;
	      }
	  }
	rewind(fp_in);

	flg.selected_dm.reserve(flg.nb_selected_dm);
	DM tmp_selected;
	while (!feof(fp_in))
	  {
	    if ( fscanf(fp_in, "%s %f %f\n", string, &tmp_selected.low, &tmp_selected.high) == 0 )
	      {
		fprintf(stderr, "failed to read input\n");
		return false;
	      }
	    if (strcmp(string, "selected_dm") == 0)
	      {
		flg.selected_dm.push_back(tmp_selected);
	      }
	  }
	rewind(fp_in);

	while (!feof(fp_in))
	  {
	    if ( fscanf(fp_in, "%s", string) == 0 )
	      {
		fprintf(stderr, "failed to read input\n");
		return false;
	      }
	    if (strcmp(string, "debug") == 0)
	      flg.enable_debug = 1;
	    if (strcmp(string, "analysis") == 0)
	      flg.enable_analysis = 1;
	    if (strcmp(string, "periodicity") == 0)
	      flg.enable_periodicity = 1;
	    if (strcmp(string, "acceleration") == 0)
	      flg.enable_acceleration = 1;
	    if (strcmp(string, "output_ffdot_plan") == 0)
	      flg.enable_output_ffdot_plan = 1;
	    if (strcmp(string, "output_fdas_list") == 0)
	      flg.enable_output_fdas_list = 1;
	    if (strcmp(string, "output_dmt") == 0)
	      flg.output_dmt = 1;
	    if (strcmp(string, "zero_dm") == 0)
	      flg.enable_zero_dm = 1;
	    if (strcmp(string, "zero_dm_with_outliers") == 0)
	      flg.enable_zero_dm_with_outliers = 1;
	    if (strcmp(string, "rfi") == 0)
	      flg.enable_rfi = 1;
	    if (strcmp(string, "oldrfi") == 0)
	      flg.enable_old_rfi = 1;
	    if (strcmp(string, "threshold") == 0)
	      flg.candidate_algorithm = 1;
	    if (strcmp(string, "baselinenoise") == 0)
	      flg.enable_sps_baselinenoise = 1;
	    if (strcmp(string, "fdas_custom_fft") == 0)
	      flg.enable_fdas_custom_fft = 1;
	    if (strcmp(string, "fdas_inbin") == 0)
	      flg.enable_fdas_inbin = 1;
	    if (strcmp(string, "fdas_norm") == 0)
	      flg.enable_fdas_norm = 1;
	    if (strcmp(string, "multi_file") == 0)
	      flg.multi_file = 1;
	    if (strcmp(string, "analysis_debug") == 0)
	      flg.analysis_debug = 1;
	    if (strcmp(string, "failsafe") == 0)
	      flg.failsafe = 1;
	    if (strcmp(string, "max_boxcar_width_in_sec") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.max_boxcar_width_in_sec) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "sigma_cutoff") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.sigma_cutoff) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }

	    if (strcmp(string, "periodicity_sigma_cutoff") == 0) {
	      if ( fscanf(fp_in, "%f", flg.periodicity_sigma_cutoff) == 0 ) {
		fprintf(stderr, "failed to read input\n");
		return false;
	      }
	    }
	    if (strcmp(string, "periodicity_harmonics") == 0) {
	      if ( fscanf(fp_in, "%d", flg.periodicity_nHarmonics) == 0 ) {
		fprintf(stderr, "failed to read input\n");
		return false;
	      }
	    }

	    if (strcmp(string, "sigma_constant") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.sigma_constant) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "narrow") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.narrow) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "wide") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.wide) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "nboots") == 0)
	      {
		if ( fscanf(fp_in, "%d", flg.nboots) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "navdms") == 0)
	      {
		if ( fscanf(fp_in, "%d", flg.navdms) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "nwindows") == 0)
	      {
		if ( fscanf(fp_in, "%d", flg.ntrial_bins) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "aggression") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.aggression) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "power") == 0)
	      {
		if ( fscanf(fp_in, "%f", flg.power) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
	      }
	    if (strcmp(string, "file") == 0)
	      {
		// this command expand "~" to "home/username/"
		wordexp_t expanded_string;

		if ( fscanf(fp_in, "%s", string) == 0 )
		  {
		    fprintf(stderr, "failed to read input\n");
		    return false;
		  }
		wordexp(string, &expanded_string, 0);
		if (( fp = fopen(expanded_string.we_wordv[0], "rb") ) == NULL)
		  {
		    fprintf(stderr, "Invalid data file!\n");
		    return false;
		  }
		wordfree(&expanded_string);
	      }
	  }

      }
    else if (argc == 2 && strcmp(argv[1], "-help") == 0)
      {
	//help();
	return false;
      }
    else
      {
	fprintf(stderr, "Cannot recognise input, try \"./astro-accelerate -help.\"\n");
	return false;
      }
	
    return true;
  }
};

#endif // ASTRO_ACCELERATE_AA_CONFIG_HPP
