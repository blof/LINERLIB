//
// C++ Interface: cl_params
//
// Description: 
//
//
// Author: berit <berit@diku.dk>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CL_PARAMETERS_HPP
#define CL_PARAMETERS_HPP

#include <string>
#include <iostream>
/**
	@author berit <blof@man.dtu.dk>
*/

using namespace std;

class cl_parameters{
public:
    cl_parameters();

    ~cl_parameters();

const char* port_file;
const char* demand_file;
const char* distance_file;
const char* fleet_file;
const char* output_file;
const char* time_file;
const char* misc1_file;
const char* misc2_file;
const char* fleet_data_file;

void print();

};

#endif
