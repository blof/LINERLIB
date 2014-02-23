/*
 * bm_rotation.h
 *
 *  Created on: Feb 23, 2014
 *      Author: berit
 */

#ifndef BM_ROTATION_H_
#define BM_ROTATION_H_

#include "bm_data.hpp"

using namespace std;
using namespace BM;

class bm_rotation {
public:
	bm_rotation(BM::vesselclass v_class, double s, double num_v, vector<int> call_sequence);

	virtual ~bm_rotation();

	/**Check the draft feasibility between vessel_class and port_calls
	 * @return true if feasible, false if not
	 */
	bool check_draft_feasibility();

	/**
	 * vessel_cost is term (2a) of the reference model (page 18)
	 * @return=f_^{v_r}*Y^r	(=T^F*180*Y_r)
	 */
	double vessel_running_cost();

	/**
	 * Function to calculate bunker cost (idle and sailing) at e=600 USD per ton
	 * Part of Equation (2b)
	 * @return bunker price = m_r*Y^r*(bunker_sailing_per_roundtrip + bunker_idle_per_roundtrip)
	 */
	double bunker_cost();

	/**
	 * Function to calculate port_call_cost for vessel class v
	 * Part of Equation d_j^{v_r}(2b)
	 * @return port_call_cost = m_r*Y^r*(port_call_cost for one round trip)
	 */
	double port_call_cost();

	/**
	 * Function to calculate port_call_cost for vessel class v
	 * Part of Equation a_{ij}^{v_r} (2b)
	 * @return canal_cost = m_r*Y^r*(canal_cost for one round trip)
	 */
	double canal_cost();


private:

	/*
	 * Helping function for bunker, canal and round trip time calculations
	 */
	void calculate_distance_and_canals();

	void calculate_round_trip_time();

	BM::vesselclass m_class;
	double m_speed;//Check if within min_max upon construction
	int m_num_v; //Y^r in model
	double m_num_round_trip; //m^r
	vector<int> m_port_calls; //E_r as integer port codes
	double m_distance;
	int m_num_suez;
	int	m_num_panama;
	double m_round_trip_time;


};

#endif /* BM_ROTATION_H_ */
