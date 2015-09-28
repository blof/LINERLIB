/*
 * bm_rotation.cpp
 *
 *  Created on: Feb 23, 2014
 *      Author: berit
 */

#include "bm_rotation.h"



rotation::rotation(BM::vesselclass v, double s, int num_v, vector<string> calls) : m_class(v), m_speed(s), m_num_v(num_v), m_port_calls(calls){
assert(m_speed >= m_class.m_min_speed);
assert(m_speed <= m_class.m_max_speed);
m_draft_feasible=false;
m_vrc=m_class.m_OPEX_cost*180.0*m_num_v;
m_bc=m_cc=m_pc=0.0;
m_num_round_trip=0.0; //initialised to zero such that if the costs are not calculated all terms in equation 2a and 2b will return zero.
m_num_calls=m_port_calls.size();
}


rotation::~rotation(){
}

double rotation::calculate_round_trip_time(){
  double sail_hours= m_distance/m_speed;
  double port_hours = 24.0*m_num_calls;
  double total_hours= sail_hours+port_hours;
  double rtt_days=total_hours/24.0;

  return rtt_days;
}

double rotation::fuel_burn(double speed){
     double base=(speed/m_class.m_design_speed);
     double rv=pow(base,3)*m_class.m_fuel_consumption;
     return rv;
}
