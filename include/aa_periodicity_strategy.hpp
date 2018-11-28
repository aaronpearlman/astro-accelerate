//
//  aa_periodicity_strategy.hpp
//  aapipeline
//
//  Created by Cees Carels on Tuesday 23/10/2018.
//  Copyright © 2018 Astro-Accelerate. All rights reserved.
//

#ifndef ASTRO_ACCELERATE_PERIODICITY_STRATEGY_HPP
#define ASTRO_ACCELERATE_PERIODICITY_STRATEGY_HPP

#include <stdio.h>

#include "aa_periodicity_plan.hpp"

namespace astroaccelerate {

  class aa_periodicity_strategy {
  public:
    aa_periodicity_strategy() : m_sigma_cutoff(0),
				m_OR_sigma_multiplier(0),
				m_nHarmonics(0),
				m_export_powers(0),
				m_candidate_algorithm(false),
				m_enable_outlier_rejection(false) {
      
    }
    
    aa_periodicity_strategy(const aa_periodicity_plan &plan) : m_sigma_cutoff(plan.sigma_cutoff()),
							       m_OR_sigma_multiplier(plan.OR_sigma_multiplier),
							       m_nHarmonics(plan.nHarmonics()),
							       m_export_powers(plan.export_powers()),
							       m_candidate_algorithm(plan.candidate_algorithm()),
							       m_enable_outlier_rejection(plan.enable_outlier_rejection()) {
      
    }
    
    float sigma_cutoff() const {
      return m_sigma_cutoff;
    }

    float OR_sigma_multiplier() const {
      return m_OR_sigma_multiplier;
    }

    int nHarmonics() const {
      return m_nHarmonics;
    }

    int export_powers() const {
      return m_export_powers;
    }

    bool m_candidate_algorithm() const {
      return m_candidate_algorithm;
    }
  
    bool enable_outlier_rejection() const {
      return m_enable_outlier_rejection;
    }
  
  private:
    float m_sigma_cutoff;
    float m_OR_sigma_multiplier;
    int   m_nHarmonics;
    int   m_export_powers;
    bool  m_candidate_algorithm;
    bool  m_enable_outlier_rejection;
  };

} //namespace astroaccelerate

#endif /* ASTRO_ACCELERATE_PERIODICITY_STRATEGY_HPP */