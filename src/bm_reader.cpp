/*
 *     <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator

#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/property_iter_range.hpp>
#include <boost/graph/graphviz.hpp>

#ifdef WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include "../include/BM_Graph.hpp"
#include "../include/bm_reader.hpp"

//#define DEBUG_OUT
//#define DIST_GEN

using namespace std;
using namespace boost;
using namespace BM;

typedef std::map < std::pair<graph_traits<BM::Graph>::vertex_descriptor, graph_traits<BM::Graph>::vertex_descriptor> , boost::tuple<double, double, double> > demand_map_t;
typedef boost::graph_traits < BM::Graph >::vertex_descriptor vertex;
typedef tokenizer< escaped_list_separator<char> > Tokenizer;

BM::reader::reader ( cl_parameters cmdline )
{
	m_file_names=cmdline;
}
BM::reader::~reader()
{
}

BM::data BM::reader::parse_instance(){

	parse_ports_to_vertices();
	parse_distances_to_edges();
	parse_demands();
	parse_vessel_classes();
	retrieve_instance_data();
	BM::data rv(true, m_graph, m_fleet, m_demand_map, m_instance_ports, m_instance_graph, m_instance_dem, m_instance_dist, m_suez, m_panama);

	return rv;
}

void BM::reader::parse_ports_to_vertices()
{
	BM::Graph::vertex_descriptor v;

	ifstream in ( m_file_names.port_file );
	if ( !in.is_open() )
	{
		cerr << "cannot open port file " << m_file_names.port_file << endl;
		exit ( 0 );
	}

	escaped_list_separator<char>  sep ( "","\t","" );
	vector< string > vec;
	string line;
	int curr_index=0;

	//Skipping Header line
	getline ( in,line );

	while ( getline ( in,line ) )
	{
		Tokenizer tok ( line, sep );
		vec.assign ( tok.begin(),tok.end() );

		// A correctly parsed line should contain 12 fields
		if ( vec.size() < 12 ){
			cerr <<" Parse_ports_to_vertices(): Malformatted line: " <<  line << endl;
			continue;
		}
		//UNLOCODE;name;country;cabotage;region;latitude;longitude;draft;moveCost;transhipmentCost;callCostFixed; callCostPerFFE
		v= add_vertex ( BM::Node ( curr_index ,vec.at ( 0 ), vec.at ( 1 ), vec.at ( 2 ), vec.at ( 3 ), vec.at ( 4 ), atof ( vec.at ( 5 ).c_str() ), atof ( vec.at ( 6 ).c_str() ), atof ( vec.at ( 7 ).c_str() ),
				round(atof ( vec.at ( 8 ).c_str() )),round(atof ( vec.at ( 9 ).c_str() )), round(atof ( vec.at ( 10 ).c_str() )),round(atof ( vec.at ( 11 ).c_str() ) ) ) , m_graph );

		//TODO: here we need to set it individually?
		if(m_graph[v].UNLOCODE == "EGSUZ" || m_graph[v].UNLOCODE == "PAPCN"){
			m_graph[v].isWayPoint=true;
			m_graph[v].isCanal=true;
		}
		else{
			m_graph[v].isWayPoint=false;
			m_graph[v].isCanal=false;
		}
		curr_index++;
		//insert into auxiliary map for easy lookup for edges
		m_UNLOCODE_to_vertex.insert ( make_pair ( m_graph[v].UNLOCODE, v ) );
#ifdef DEBUG_OUT
		if(m_graph[v].UNLOCODE == "EGSUZ" || m_graph[v].UNLOCODE == "PAPCN"){
			copy ( vec.begin(), vec.end(),ostream_iterator<string> ( cout, "|" ) );
			cout << m_graph[v].isCanal << endl;
		}
#endif
	}
}

void BM::reader::parse_distances_to_edges()
{
	edge_descriptor e;
	vertex_descriptor u,v;

	ifstream in ( m_file_names.distance_file );
	if ( !in.is_open() )
	{
		cerr << "cannot open distance file " << m_file_names.distance_file << endl;
		exit ( 0 );
	}
	escaped_list_separator<char> sep ( "","\t","" );
	vector< string > vec;
	string line;
	int curr_index=0;

	//Skipping Header line
	getline ( in,line );

	while ( getline ( in,line ) )
	{
		Tokenizer tok ( line, sep );
		vec.assign ( tok.begin(),tok.end() );

		// A correctly parsed line should contain 6 fields
		if ( vec.size() < 6 ){
			cerr << "parse_distances_to_edges()  - Malformatted Line: " << line << endl;
			continue;
		}
		//find the vertices
		u=UNLOCODE_to_vertex ( vec.at ( 0 ) );//From
		v=UNLOCODE_to_vertex ( vec.at ( 1 ) );//To
		if(u == -1 || v == -1)	  continue;
		//index, dist, draft, suez, panama
		add_edge ( u, v,
				BM::Edge ( curr_index, atof ( vec.at ( 2 ).c_str() ), atof ( vec.at ( 3 ).c_str() ), ( atof ( vec.at ( 5 ).c_str() ) ==1 ), ( atof ( vec.at ( 4 ).c_str() ) ==1 ) ), m_graph );
		curr_index++;

#ifdef DEBUG_OUT
		copy ( vec.begin(), vec.end(),ostream_iterator<string> ( cout, "|" ) );
#endif
	}

#ifdef DEBUG_OUT
	cout << endl << "vertices " << num_vertices ( m_graph ) << " edges " << num_edges ( m_graph ) << endl;
#endif

}

void BM::reader::parse_demands()
{
	vertex_descriptor o,d;
	map<string, vertex_descriptor>::iterator it, it_end;
	it_end=m_UNLOCODE_to_vertex.end();

	ifstream in ( m_file_names.demand_file );
	if ( !in.is_open() )
	{
		cerr << "cannot open demand file " << m_file_names.demand_file << endl;
		exit ( 0 );
	}
	escaped_list_separator<char> sep ( "","\t","" );
	vector< string > vec;
	string line;

	//Skipping Header line
	getline ( in,line );

	while ( getline ( in,line ) )
	{
		Tokenizer tok ( line, sep );
		vec.assign ( tok.begin(),tok.end() );

		// A correctly parsed line should contain 6 fields
		if ( vec.size() < 5 ){
			cerr <<" parse_demands() - Malformatted line: " << line << endl;
			continue;
		}
		if( vec.at ( 0 ) == vec.at ( 1 ) ){
			cerr << " Invalid demand  " << vec.at ( 0 ) << "->" << vec.at ( 1 ) << endl;
			exit(0);
		}
		//find vertices
		o=UNLOCODE_to_vertex ( vec.at ( 0 ) );
		d=UNLOCODE_to_vertex ( vec.at ( 1 ) );
		if(o == -1){
			cerr << "In bm_reader::parse_demands: origin "<< vec.at(0) << " not found" << endl;
			cerr.flush();
			exit(0);
		}
		if(d == -1){
			cerr << "In bm_reader::parse_demands: dest "<< vec.at(1) << " not found" << endl;
			cerr.flush();
			exit(0);
		}


		pair<vertex_descriptor, vertex_descriptor> key=  make_pair ( o,d );
		boost::tuple<double, double, double> value = make_tuple ( atof ( vec.at ( 2 ).c_str() ),atof ( vec.at ( 3 ).c_str() ),atof ( vec.at ( 4 ).c_str() ) );
		pair<int,int> key_idx=make_pair(m_graph[o].idx,m_graph[d].idx);
		m_demand_map.insert ( make_pair ( key, value ) );
		m_instance_dem.insert( make_pair(key_idx, value));
	}
}

void BM::reader::parse_vessel_classes()
{
	ifstream in (m_file_names.fleet_data_file);
	if ( !in.is_open() )
	{
		cerr << "cannot open fleet file " << m_file_names.fleet_data_file << endl;
		exit ( 0 );
	}

	escaped_list_separator<char>  sep ( "","\t","" );
	vector< string > vec;
	string line;
	int index=0;

	//Skipping Header line
	getline ( in,line );

	while ( getline ( in,line ) )
	{
		Tokenizer tok ( line, sep );
		vec.assign ( tok.begin(),tok.end() );

		// A correctly parsed line should contain 11 fields
		if ( vec.size() < 10 ){
			cerr << "Parse_vessel_classes()  - Malformatted line: "<<line << endl;
			continue;
		}
		//Name, Cap, TC, Draft, Min s, max s, s* ,bunker, quantity, suez cost, panama cost
		m_fleet.push_back(
				BM::vesselclass (
						index, vec.at ( 0 ), atoi ( vec.at ( 1 ).c_str() ), atoi ( vec.at ( 2 ).c_str() ), atof ( vec.at ( 3 ).c_str() ), atof ( vec.at ( 4 ).c_str() ), atof ( vec.at ( 5 ).c_str() ), atof ( vec.at ( 6 ).c_str() ), atof ( vec.at ( 7 ).c_str() ),atof ( vec.at ( 8 ).c_str() ), atof(vec.at(9).c_str()), atof(vec.at(10).c_str()) ));
		index++;
	}

	//map of values
	map<string,int> vessel_name_pos;
	vessel_name_pos.insert(make_pair("Feeder_450",0));
	vessel_name_pos.insert(make_pair("Feeder_800",1));
	vessel_name_pos.insert(make_pair("Panamax_1200",2));
	vessel_name_pos.insert(make_pair("Panamax_2400",3));
	vessel_name_pos.insert(make_pair("Post_panamax",4));
	vessel_name_pos.insert(make_pair("Super_panamax",5));

	ifstream quan ( m_file_names.fleet_file );
	if ( !quan.is_open() )
	{
		cerr << "cannot open fleet instance file " << m_file_names.fleet_file  << endl;
		exit ( 0 );
	}

	//Skipping Header line
	getline ( quan,line );

	while ( getline ( quan,line ) )
	{
		Tokenizer tok ( line, sep );
		vec.assign ( tok.begin(),tok.end() );

		// A correctly parsed line should contain type,quantity fields
		if ( vec.size() < 2 ){
			cerr <<"Parse_vessel_classes() - fleet_INSTANCE.csv: Malformatted line: " <<   line << endl;
			continue;
		}
		switch(vessel_name_pos[vec.at(0)]){
		case 0:
			m_fleet.at(0).set_qnt(atoi(vec.at(1).c_str()));
			break;
		case 1:
			m_fleet.at(1).set_qnt(atoi(vec.at(1).c_str()));
			break;
		case 2:
			m_fleet.at(2).set_qnt(atoi(vec.at(1).c_str()));
			break;
		case 3:
			m_fleet.at(3).set_qnt(atoi(vec.at(1).c_str()));
			break;
		case 4:
			m_fleet.at(4).set_qnt(atoi(vec.at(1).c_str()));
			break;
		case 5:
			m_fleet.at(5).set_qnt(atoi(vec.at(1).c_str()));
			break;
		default:
			cerr << "Unknown vessel class " << vec.at(0) << endl;
			abort();

		}
	}
}

void BM::reader::retrieve_instance_data()
{
	demand_map_t::iterator it, itend;
	itend=m_demand_map.end();
	pair<int, string> port_key_1, port_key_2;
	//Finding the ports that are used in the instance to serve the demands
	for ( it=m_demand_map.begin(); it!=itend; it++ )
	{
		pair<vertex_descriptor, vertex_descriptor> key=it->first;
		port_key_1=make_pair ( m_graph[key.first].idx, m_graph[key.first].UNLOCODE );
		port_key_2=make_pair ( m_graph[key.second].idx, m_graph[key.second].UNLOCODE );
		m_instance_ports.insert ( port_key_1 );
		m_instance_ports.insert ( port_key_2 );
	}

	m_instance_graph=m_graph;
	//Now separate the distances needed by an integer idx to have efficient retrieval of the distances
	port_map_t::iterator p_it, p_to_it, p_itend;
	vertex_descriptor u,v;
	Viterator vi, vi_end;
	vector<uint> to_be_cleared;
	vector<uint>::iterator remove_it, rem_end;
	p_itend=m_instance_ports.end();
	//Clear and renmove vertices not in the instance
	for (tie(vi, vi_end) = vertices(m_instance_graph); vi != vi_end; ++vi){
		v=*vi;
		p_it=m_instance_ports.find(m_instance_graph[v].idx);
		//Keep Canals in the instance
		if( m_instance_graph[v].UNLOCODE == "EGSUZ"){
			m_suez=make_pair(m_instance_graph[v].callCostFixed, m_instance_graph[v].callCostPerFFE);
			continue;
		}
		if( m_instance_graph[v].UNLOCODE == "PAPCN"){
			m_panama=make_pair( m_instance_graph[v].callCostFixed, m_instance_graph[v].callCostPerFFE);
			continue;
		}
		if(p_it==p_itend){
			to_be_cleared.push_back(m_instance_graph[v].idx);
		}
	}
	rem_end=to_be_cleared.end();
	for ( remove_it= to_be_cleared.begin(); remove_it != rem_end; remove_it++){
		for (tie(vi, vi_end) = vertices(m_instance_graph); vi != vi_end; vi++){
			if(*remove_it == m_instance_graph[*vi].idx){
				clear_vertex(*vi,m_instance_graph);
				remove_vertex(*vi,m_instance_graph);
				break;
			}
		}
	}

	//Reset vertice idx's
	int id = 0;
	std::map <int, int> idXMapping;
	for (tie(vi, vi_end) = vertices(m_instance_graph); vi != vi_end; id++,vi++){
		idXMapping.insert(make_pair (m_instance_graph[*vi].idx,id));
		m_instance_graph[*vi].idx = id;
	}

	//Reset Demand Idx's
	demand_idx_map_t::iterator d_it,  d_itend;
	demand_idx_map_t temp_instance_dem;
	d_itend = m_instance_dem.end();
	for (d_it = m_instance_dem.begin(); d_it != d_itend; d_it++){
		temp_instance_dem.insert(make_pair(make_pair(idXMapping[d_it->first.first],idXMapping[d_it->first.second]),d_it->second));
	}
	m_instance_dem.clear();
	m_instance_dem = temp_instance_dem;

	//Reset m_instance_ports
	port_map_t::iterator pp_it,  pp_itend;
	port_map_t temp_instance_ports;
	pp_itend = m_instance_ports.end();
	for (pp_it = m_instance_ports.begin(); pp_it != pp_itend; pp_it++){
		temp_instance_ports.insert(make_pair(idXMapping[pp_it->first],pp_it->second));
	}
	m_instance_ports.clear();
	m_instance_ports = temp_instance_ports;




	//Making all to all distances for ports included in the instance
	Eiterator ei, ei_end;
	data_map_t::iterator dist_it, dist_it_end;
	dist_it_end=m_instance_dist.end();

	for (tie(ei, ei_end) = edges(m_instance_graph); ei != ei_end; ei++){
		v=source(*ei,m_instance_graph);
		u=target(*ei,m_instance_graph);



		pair<int, int> key=make_pair ( m_instance_graph[v].idx, m_instance_graph[u].idx);
		m_instance_dist.insert ( make_pair ( key  , make_tuple(m_instance_graph[*ei].distance, m_instance_graph[*ei].isSuezTraversal, m_instance_graph[*ei].isPanamaTraversal)) );
	}
#ifdef DIST_GEN
	cout << "Reading the mapping file." << endl ;
	multimap<string, string> port_codes;
	escaped_list_separator<char>  sep ( "","\t","" );
	vector< string > fields;
	string line;
	ifstream portmap("portidmapping.txt");
	//Skipping Header line
	getline ( portmap,line );
	while ( getline ( portmap,line ) )
	{
		Tokenizer tok ( line, sep );
		fields.assign ( tok.begin(),tok.end() );

		// A correctly parsed line should contain 3 fields
		if ( fields.size() < 2 )
			cout << line << endl;
		else {//REVERSED TO HAVE UNLOCODE, rkst
			port_codes.insert(make_pair(fields.at(1), fields.at(0)));
			cout << "inserting " << fields.at(1) <<" " << fields.at(0) << endl;

		}

	}
	multimap<string, string>::iterator pm_it, pm_it_end;
	pm_it_end=port_codes.end();
	//calculate port call cost for every vertex for every vessel class

	ofstream dist("dist_NET.csv");
	port_map_t::iterator pp_sec;
	//  data_map_t::iterator dist_it;
	pair<int,int> key;
	for (pp_it = m_instance_ports.begin(); pp_it != pp_itend; pp_it++) {
		pp_sec=pp_it;
		pp_sec++;
		for (pp_sec; pp_sec != pp_itend; ++pp_sec) {

			key=make_pair(pp_it->first, pp_sec->first);
			dist_it=m_instance_dist.find(key);
			pm_it=port_codes.find(pp_it->second);
			string rkst_1,rkst_2;
			if (pm_it!=pm_it_end) {
				rkst_1=pm_it->second;
			}else{
				cout << pp_it->second << " NOT FOUND in PORTIDMAPPING " << endl;
			}
			pm_it=port_codes.find(pp_sec->second);
			if (pm_it!=pm_it_end) {
				rkst_2=pm_it->second;
			}else{
				cout << pp_sec->second << " NOT FOUND in PORTIDMAPPING " << endl;
			}
			//    dist << pp_it->second << "\t" << pp_sec->second << "\t" << get<0>(dist_it->second)<< endl;
			//    dist << pp_sec->second << "\t" << pp_it->second << "\t" << get<0>(dist_it->second)<< endl;
			dist << rkst_1 << "\t" << rkst_2 << "\t" << get<0>(dist_it->second)<< endl;
			dist << rkst_2 << "\t" << rkst_1 << "\t" << get<0>(dist_it->second)<< endl;
		}
	}


#endif


}


vertex_descriptor BM::reader::UNLOCODE_to_vertex ( string UNLOCODE )
{
	vertex_descriptor rv;
	map<string, vertex_descriptor>::iterator it, it_end;
	it_end=m_UNLOCODE_to_vertex.end();
	//find the vertices
	it=m_UNLOCODE_to_vertex.find ( UNLOCODE );
	if ( it != it_end )
	{
		rv=it->second;
	}
	else
	{
		cout << "The port " << UNLOCODE << " is not defined!" << endl;
		return -1;
		//exit ( 0 );
	}
	return rv;
}

void BM::reader::test_bundled_properties()
{

	vertex_descriptor vd;
	edge_descriptor ed;
	graph_traits<BM::Graph>::edge_iterator e_it, e_it_end;
	for ( tie ( e_it, e_it_end ) = edges ( m_graph ); e_it != e_it_end; ++e_it )
	{
		ed=*e_it;
		const Edge& node_prop = get ( edge_bundle, m_graph ) [ed];
		vd=target ( ed, m_graph );
		const Node& vert_prop = get ( vertex_bundle, m_graph ) [vd];

		cout << "Edge Properties: distance " << node_prop.distance << " draft " << node_prop.draft << endl;
		cout << "vertex_properties: name " << vert_prop.name << " port call cost " << vert_prop.callCostFixed << endl;
	}

}

//TODO: Refactor into a writer for NET: Port call cost is calculated.
//NB! if instance specific use m_instance_graph
void BM::reader::make_port_call_costs(){
	vertex_descriptor v;
	Viterator vi, vi_end;

	ofstream out_file("pcc.csv");
	vector<vesselclass>::iterator it, it_end;
	it_end=m_fleet.end();
	//calculate port call cost for every vertex for every vessel class
	for (tie(vi, vi_end) = vertices(m_graph); vi != vi_end; ++vi){
		v=*vi;
		for(it=m_fleet.begin(); it != it_end ; it++){
			//calculate port call cost
			double pcc=m_graph[v].callCostFixed + m_graph[v].callCostPerFFE * it->m_capacity;
			out_file << m_graph[v].UNLOCODE <<"\t" << it->m_name <<"\t" << pcc << "\t23 \t" << it->m_capacity*2<< endl;
		}
	}
}

//TODO: Refactor into a writer for NET: Port call cost is calculated.
//NB! if instance specific use m_instance_graph
void BM::reader::make_vessel_class_bunker_consumption(){
	vector<vesselclass>::iterator it, it_end;
	it_end=m_fleet.end();
	ofstream out_file("vcbun.csv");
	//calculate port call cost for every vertex for every vessel class
	for(it=m_fleet.begin(); it != it_end ; it++){

		double min_s=it->m_min_speed;
		double max_s=it->m_max_speed+0.5;
		double speed=min_s;
		while(speed <max_s ){
			//calculate bunker consumption at speed
			double base=(speed/it->m_design_speed);
			double fuel=pow(base,3)*it->m_fuel_consumption;

			out_file<< it->m_name << "\t" << speed << "\t" << fuel << endl;
			speed+=0.5;
		}
	}
}

/*
//TODO: this function should be replaced by a JSON reader/writer, but for now using this to validate results

void reader::read_OPTIMIZEDinstance(const char* networkfile) {

	int num_v;
	int capacity;
	BM::vesselclass v;
	int index;
	const char* ch;
	string name;
	map<string, int> service_name_to_index;

	string service_name;
	vector<string> calls;
	vector<int> port_ids;

	BM::vertex_descriptor p;
	BM::Node N;
	string UNLOCODE;
	bool found;

	ifstream in ( networkfile);
	if ( !in.is_open() )
	{
		cerr << "cannot open optimized network file " << networkfile << endl;
		exit ( 0 );
	}

	escaped_list_separator<char>  sep_service ( "","\t","" );
	escaped_list_separator<char>  sep_info ( ""," ","" );

	vector< string > fields_info, fields_service, fields;
	string line;

	//Skipping 5 Header lines
	getline ( in,line );
	getline ( in,line );
	getline ( in,line );
	getline ( in,line );
	getline ( in,line );


	while ( getline ( in,line ) )
	{
		Tokenizer tok_service ( line, sep_service );
		Tokenizer tok_info ( line, sep_info );

		fields_info.assign ( tok_info.begin(),tok_info.end() );
		if(fields_service.size()!=3){
			//cout << "service id: " << fields_info.at(1)<<endl;
			name=fields_info.at(1);
			ch=fields_info.at(1).c_str();
			index=atoi(ch);

			getline ( in,line );
			fields_info.assign ( tok_info.begin(),tok_info.end() );
			//cout << "capacity: " << fields_info.at(1)<<endl;
			ch=fields_info.at(1).c_str();
			capacity=atoi(ch);

			getline ( in,line );
			fields_info.assign ( tok_info.begin(),tok_info.end() );
			//cout << "no vessels: " << fields_info.at(3)<<endl;
			ch=fields_info.at(3).c_str();
			num_v=atoi(ch);
			v=get_vessel_class(capacity);

			getline ( in,line );
			Tokenizer tok_service ( line, sep_service );
			fields_service.assign ( tok_service.begin(),tok_service.end() );
			if(fields_service.size()!=3){
				//cout<<"Butterfly = 1 "<<endl;
				getline ( in,line );
				Tokenizer tok_info ( line, sep_info );
				fields_service.assign ( tok_service.begin(),tok_service.end() );
			}
			else{
				//cout << "Butterfly = 0"<<endl;
			}
		}

		while(fields_service.size()==3){
			//cout << "Port id " << fields_service.at(0)<<endl;
			ch=fields_service.at(0).c_str();
			port_ids.push_back (atoi(ch));

			//cout << "Unlocode  " << fields_service.at(1)<<endl;
			UNLOCODE=fields_service.at(1);

			tie(found, p)= UNLOCODE_to_vertex(UNLOCODE);
			if(!found){
				cerr << "In data::read_OPTIMIZEDinstance: ";
				cerr << "UNLOCODE "  << UNLOCODE << endl;
				cerr << " could not find " << UNLOCODE	<< " as a vertex " << endl;
			}
			N=m_instance_graph[p];

			calls.push_back(N.UNLOCODE);
			//cout << "sammenligning af ider "<< N.idx << "vs" << atoi(ch) << endl;

			//cout << "Port " << fields_service.at(2)<<endl;
			getline ( in,line );
			Tokenizer tok_service ( line, sep_service );
			Tokenizer tok_info ( line, sep_info );
			fields_service.assign ( tok_service.begin(),tok_service.end() );
			fields_info.assign ( tok_info.begin(),tok_info.end() );

			//cout <<"size " << fields_info.size() << " empty " << fields_info.empty() <<" cap " << fields_info.capacity() << endl;
			//if(fields_info.size()==0){
			if(line.length()<=1){
				getline ( in,line );
				Tokenizer tok_service ( line, sep_service );
				Tokenizer tok_info ( line, sep_info );
				fields_service.assign ( tok_service.begin(),tok_service.end() );
				fields_info.assign ( tok_info.begin(),tok_info.end() );
				break;
			}
		}

		ch=fields_info.at(1).c_str();
		double speed=atof(ch);
		cout << "Speed: " << speed <<endl;
		getline ( in,line );
		Tokenizer tok1 ( line, sep_info );
		fields.assign ( tok1.begin(),tok1.end() );
//		cout << "Distance: " << fields.at(5) << endl;
		getline ( in,line );
		fields.assign ( tok1.begin(),tok1.end() );
//		cout << "Duration: " << fields.at(5)<<endl;

		rotation rv= rotation ( v, speed, num_v , calls );
		m_rotations.push_back(rv);

		calls.clear();
		port_ids.clear();

		//break;
	}
	in.close();
}

vesselclass reader::get_vessel_class(int capacity) {


	vesselclass v;
	  vector<vesselclass>::iterator it, it_end;
	  it_end=m_fleet.end();
	  for ( it=m_fleet.begin(); it != it_end; it++ )
	    {
		  v=*it;
		  if(it->m_capacity == capacity)	break;
	    }

	return v;
}
*/

