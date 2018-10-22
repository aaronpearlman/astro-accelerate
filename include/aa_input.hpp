#ifndef ASTRO_ACCELERATE_AA_INPUT_HPP
#define ASTRO_ACCELERATE_AA_INPUT_HPP

#include "aa_filterbank_metadata.hpp"

class aa_input {
public:
  aa_input();
  ~aa_input();

  virtual bool open();
  virtual bool close();
  virtual bool read();
  virtual aa_filterbank_metadata metadata() const = 0;
protected:
  virtual bool init();
};

#endif // ASTRO_ACCELERATE_AA_INPUT_HPP
