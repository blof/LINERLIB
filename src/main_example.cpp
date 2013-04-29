#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>

#include <boost/foreach.hpp>
#include <boost/concept/assert.hpp>


#include "cl_parameters.hpp"
#include "bm_reader.hpp"
#define DEBUG_instance //check if instance is parsed correctly

using namespace std;
using namespace boost;


void display_usage ( void )
{
	cout << "Usage:" << endl;
	cout << "./APPLIKATION NAME ..." << endl; //Change this to the name of your binary!
	cout << "-p port file" << endl;
	cout << "-d demand file" << endl;
	cout << "-t distance file" << endl;
	cout << "-f fleet data file" << endl;
	cout << "-q fleet file" << endl;
	cout << "-o outputfile" << endl;
	cout << "-b timefile" <<"-? -h display usage" << endl;
	/* ... */
	exit ( EXIT_FAILURE );
}

int main ( int argc, char* argv[] )
{
	cl_parameters cl_params; //command line parameters

	if ( argc > 13 ) //If you extend the number of cases and arguments please increase this number
	{
		cerr << "Wrong Number of arguments" << endl;
		display_usage();
		exit ( EXIT_FAILURE );
		
	}
	
	
	for ( int i = 1; i+1 < argc; i+=2 )
	{
		switch ( argv[i][1] )
		{
			case 'p':
				cl_params.port_file = argv[i+1];
				break;
			case 'd':
				cl_params.demand_file = argv[i+1];
				break;
			case 't':
				cl_params.distance_file = argv[i+1];
				break;
			case 'f':
				cl_params.fleet_file = argv[i+1];
				break;
			case 'q':
				cl_params.fleet_data_file = argv[i+1];
				break;
			case 'o':
				cl_params.output_file = argv[i+1];
				break;
			case 'b':
				cl_params.time_file = argv[i+1];
				break;
			case 'h':
			case '?':
				display_usage();
				break;
			default:
				/* Shouldn't get here */
				cerr << "Unknown problem" << endl;
				display_usage();
				break;
		}
	}

#ifdef DEBUG_instance
	cout << "Command line parameters: " << endl;
	cl_params.print();
#endif
	//parse benchmark instance
	BM::reader bm_reader ( cl_params );
	
	BM::data instanceBM=bm_reader.parse_instance();
#ifdef DEBUG_instance
	instanceBM.print_inst_graph ( "graph_inst.dot" );
	instanceBM.print_demands_by_id();
	instanceBM.print_fleet();
#endif
	
return 0;
}
