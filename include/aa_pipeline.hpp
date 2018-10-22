#ifndef ASTRO_ACCELERATE_AA_PIPELINE_HPP
#define ASTRO_ACCELERATE_AA_PIPELINE_HPP

#include <vector>

enum class modules : int {
    analysis = 0,
    acceleration,
    periodicity,
    dmt,
    zero_dm,
    zero_dm_with_outliers,
    rfi,
    old_rfi,
    sps_baseline_noise,
    fdas_custom_fft,
    fdas_inbin,
    fdas_norm,
    output_ffdot_plan,
    output_fdas_list
};

enum class debug : int {
    debug = 0,
    analysis
};

class aa_pipeline {
public:
  void pipeline(const std::vector<modules> &list) {
    lst = list;
  }

protected:
  std::vector<modules> lst; // List of pipeline modules to be executed
};

#endif // ASTRO_ACCELERATE_AA_PIPELINE_HPP
