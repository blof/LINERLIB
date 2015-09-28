/*
 * bm_rotation.h
 *
 *  Created on: Feb 23, 2014
 *      Author: berit
 */

#ifndef BM_ROTATION_H_
#define BM_ROTATION_H_

#include <boost/config.hpp>
#include "bm_data.hpp"

using namespace std;
using namespace BM;

class rotation {
public:

	rotation( BM::vesselclass v, double s, int num_v, vector<string> call_sequence);

	~rotation();

	/**There must be draft feasibility between vessel_class and port_calls
	 * @return true if feasible, false if not
	 */
	bool is_draft_feasible(){
		return m_draft_feasible;
	}

	void set_draft_feasibility(bool is_draft_feasible){
		m_draft_feasible=is_draft_feasible;
	}

	/**
	 * vessel_cost is term (2a) of the reference model (page 18)
	 * @return=f_^{v_r}*Y^r	(=T^F*180*Y_r)
	 */
	double get_vessel_running_cost(){
		return m_vrc;
	}

	/**
	 * set vessel_cost is term (2a) of the reference model (page 18)
	 * @param=f_^{v_r}*Y^r	(=T^F*180*Y_r)
	*/
	void set_vessel_running_cost(double VRC){
		m_vrc=VRC;
	}

	/**
	 * Function to calculate bunker cost (idle and sailing) at e=600 USD per ton
	 * Part of Equation (2b)
	 * @return bunker price = m_r*Y^r*(bunker_sailing_per_roundtrip + bunker_idle_per_roundtrip)
	 */
	double get_bunker_cost(){
		return m_bc;
	}

	/**
	 * set bunker cost (idle and sailing) at e=600 USD per ton
	 * Part of Equation (2b)
	 * @param bunker price = m_r*Y^r*(bunker_sailing_per_roundtrip + bunker_idle_per_roundtrip)
	 */
	void set_bunker_cost(double bc){
		m_bc=bc;
	}


	/**
	 * get port_call_cost for vessel class v
	 * Part of Equation d_j^{v_r}(2b)
	 * @return port_call_cost = m_r*Y^r*(port_call_cost for one round trip)
	 */
	double get_port_call_cost(){
		return m_pc;
	}

	/**
	 * set port_call_cost for vessel class v
	 * Part of Equation d_j^{v_r}(2b)
	 * @param port_call_cost = m_r*Y^r*(port_call_cost for one round trip)
	 * set in validator class
	 */
	void set_port_call_cost(double pc){
		m_pc=pc;
	}


	/**
	 * Return the port_call_cost for vessel class v
	 * Part of Equation a_{ij}^{v_r} (2b)
	 * @return canal_cost = m_r*Y^r*(canal_cost for one round trip)
	 */
	double get_canal_cost(){
		return m_cc;
	}


	/**
	 * set canal cost for vessel class v
	 * Part of Equation a_{ij}^{v_r} (2b)
	 * @param canal_cost = m_r*Y^r*(canal_cost for one round trip)
	 */
	void set_canal_cost(double cc){
		m_cc=cc;
	}

	double getNumRoundTrip() const {
		return m_num_round_trip;
	}

	void setNumRoundTrip(double numRoundTrip) {
		m_num_round_trip = numRoundTrip;
	}

	int getNumV() const {
		return m_num_v;
	}

	void setNumV(int numV) {
		m_num_v = numV;
	}

	const vector<string>& getPortCalls() const {
		return m_port_calls;
	}

	void setPortCalls(const vector<string>& portCalls) {
		m_port_calls = portCalls;
	}

	double getRoundTripTime() const {
		return m_round_trip_time;
	}

	void setRoundTripTime(double roundTripTime) {
		m_round_trip_time = roundTripTime;
	}

	double getSpeed() const {
		return m_speed;
	}

	void setSpeed(double speed) {
		m_speed = speed;
	}

	double getDistance() const {
		return m_distance;
	}

	void setDistance(double distance) {
		m_distance = distance;
	}

	int getNumPanama() const {
		return m_num_panama;
	}

	void setNumPanama(int numPanama) {
		m_num_panama = numPanama;
	}

	int getNumSuez() const {
		return m_num_suez;
	}

	void setNumSuez(int numSuez) {
		m_num_suez = numSuez;
	}

	/*Calculates the round trip time
	 * sail hours + port hours
	 * @return Round trip time  in DAYS
	 */
	double calculate_round_trip_time();


	/**
	 * Function to calculate bunker cost (idle and sailing) at e=600 USD per ton
	 * Part of Equation (2b)
	 * @return bunker price = m_r*Y^r*(bunker_sailing_per_roundtrip + bunker_idle_per_roundtrip)
	 */

	double calculate_bunker_cost(){
		 double rv=0;
		  //daily fuel burn* days to sail actual speed m_actual_speed and current distance
		  double rt_fuel_burn= fuel_burn (m_speed )*( (m_distance/m_speed) /24.0 ); //ton per day
		  double idle_burn= m_class.m_idle_consumption * m_num_calls; //ton per day
		  //Bc for one round trip
		  double  bc= 600 *  (rt_fuel_burn + idle_burn);
		  //m_r*Y_r*bc
		  rv=m_num_round_trip*m_num_v*bc;

		  return rv;
	}

	//Helping function Page 15, BM Paper F_s = (s/v_∗^F)^3*f_∗^F (Fuel(speed)=(speed/design_speed)^3*Fuel(design_speed)
	double fuel_burn(double speed);

	/**
	 * Function to calculate port_call_cost for vessel class v
	 * Part of Equation a_{ij}^{v_r} (2b)
	 * @return canal_cost = m_r*Y^r*(canal_cost for one round trip)
	 */

	double calculate_canal_cost(){
		double rv=m_num_suez*m_class.m_suez_cost;
		rv += m_num_panama*m_class.m_panama_cost;
		m_cc=rv;
		return rv;
	}

	const vesselclass& getClass() const {
		return m_class;
	}

	void setClass(const vesselclass& clazz) {
		m_class = clazz;
	}

private:

	BM::vesselclass m_class;
	double m_speed;//Check if within min_max upon construction
	int m_num_v; //Y^r in model
	double m_num_round_trip; //m^r
	vector<string> m_port_calls; //E_r as UNLOCODE port codes
	bool m_draft_feasible;
	double m_vrc;
	double m_bc;
	double m_cc;
	double m_pc;

	int m_num_calls;
	double m_distance;
	int m_num_suez;
	int	m_num_panama;
	double m_round_trip_time;


};

#endif /* BM_ROTATION_H_ */
