#ifndef ASTRO_ACCELERATE_FDAS_UTIL_HPP
#define ASTRO_ACCELERATE_FDAS_UTIL_HPP

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "fdas_util.hpp"
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
  int search;
  int wsig;
  int basic;
  int kfft;
  char * afname;
  char * nfname;
  float freq0;
  float thresh;
  int mul;
  int zval;
  int writef;
  float duty;
  int iter;
  int nsig;
  int nharms;
  int devid;
  int inbin;
  int norm;
  float sigamp;
} cmd_args;

  //Function declarations
extern "C" {
void print_usage(); 

void read_command_line(int argc, char *argv[], cmd_args *args);

int read_input_file(char *fname, float **array);

void read_input_file_cpx(char *fname, float2 *array);

void write_output_file(char *fname, float **array, unsigned long len);
}

#endif /* FDAS_UTIL_H */
