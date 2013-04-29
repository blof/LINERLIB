//
// C++ Implementation: cl_parameters
//
// Description: 
//
//
// Author: berit <berit@diku.dk>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "../include/cl_parameters.hpp"

using namespace std;

cl_parameters::cl_parameters()
{
  output_file= "pbls_out.txt";
  time_file="pbls_times_out.txt";
}


cl_parameters::~cl_parameters()
{
}


void cl_parameters::print(){
cout << "ports " << port_file <<
" demands " << demand_file <<
" distances " << distance_file <<
" vessels " << fleet_file <<
" quantities " << fleet_data_file <<
endl;
}
