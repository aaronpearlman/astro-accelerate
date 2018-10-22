#include "aa_sigproc_input.hpp"

aa_sigproc_input::aa_sigproc_input(const std::string &path) {

}

aa_sigproc_input::~aa_sigproc_input() {

}

bool aa_sigproc_input::open() {
  init();
  return false;
}

bool aa_sigproc_input::close() {
  return false;
}

bool aa_sigproc_input::read() {
  return false;
}

bool aa_sigproc_input::init() {
  return false;
}
