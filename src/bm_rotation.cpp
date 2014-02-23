/*
 * bm_rotation.cpp
 *
 *  Created on: Feb 23, 2014
 *      Author: berit
 */

#include "bm_rotation.h"

bm_rotation::bm_rotation(vesselclass v_class, double s, double num_v, vector<int> calls): m_class(v_class), m_speed(s), m_num_v(num_v), m_port_calls(calls){
assert(m_speed >= m_class.m_min_speed);
assert(m_speed <= m_class.m_max_speed);
calculate_distance_and_canals();
calculate_round_trip_time;
m_num_round_trip=180.0/m_round_trip_time;

}


void bm_rotation::calculate_distance_and_canals(){

}

void bm_rotation::calculate_round_trip_time(){
	double rv=0.0;
	m_round_trip_time=rv;
}
