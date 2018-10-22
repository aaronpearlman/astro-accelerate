#ifndef ASTRO_ACCELERATE_AA_SIGPROC_INPUT_HPP
#define ASTRO_ACCELERATE_AA_SIGPROC_INPUT_HPP

#include "aa_input.hpp"
#include <string>

class aa_sigproc_input : public aa_input {
public:
  aa_sigproc_input(const std::string &path); 
  ~aa_sigproc_input();
  bool open();
  bool close();
  bool read();

private:
  bool init();
  bool get_file_data();
  bool allocate_memory_cpu_input();

};

#endif // ASTRO_ACCELERATE_AA_SIGPROC_INPUT_HPP
