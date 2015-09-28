/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include "../include/BM_Graph.hpp"
#include "../include/bm_data.hpp"
#include "../include/cl_parameters.hpp"
#include "bm_rotation.h"
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#ifdef WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#ifndef BM_READER_HPP
#define BM_READER_HPP

using namespace std;
using namespace boost;
typedef std::map < std::pair<graph_traits<BM::Graph>::vertex_descriptor, graph_traits<BM::Graph>::vertex_descriptor> , boost::tuple<double, double, double> > demand_map_t;
typedef std::map < std::pair<int, int> , boost::tuple<double, double, double> > demand_idx_map_t;
// typedef std::tr1::unordered_map<std::pair<int, int>, boost::tuple<double, bool, bool> > data_map_t;
// typedef std::tr1::unordered_map<int, string> port_map_t;
typedef std::multimap<std::pair<int, int>,boost::tuple<double, bool, bool> > data_map_t;
typedef std::map<int, string> port_map_t;


namespace BM
{
class reader
{
public:
	reader ( cl_parameters );
	~reader();

	BM::data parse_instance();
	void parse_ports_to_vertices();
	void parse_distances_to_edges();
	void parse_demands();
	void parse_vessel_classes();
	void retrieve_instance_data();
	void make_port_call_costs();
	void make_vessel_class_bunker_consumption();
	const Graph get_graph()
	{
		return m_graph;
	}

	const Graph get_instance_graph()
	{
		return m_instance_graph;
	}

	vector<vesselclass> get_fleet()
    		  {
		return m_fleet;
    		  }

	const demand_map_t get_demands()
	{
		return m_demand_map;
	}

	const demand_idx_map_t get_demands_by_id()
	{
		return m_instance_dem;
	}

	const data_map_t get_distances_by_id()
	{
		return m_instance_dist;
	}

	const map<string, vertex_descriptor> get_UNLOCODE_to_vertex()
    		  {
		return m_UNLOCODE_to_vertex;
    		  }

	const cl_parameters get_file_names()
	{
		return m_file_names;
	}


	port_map_t get_ports()
	{
		return m_instance_ports;
	}

//	void read_OPTIMIZEDinstance(const char* networkfile);

private:
	cl_parameters m_file_names;
	Graph m_graph; //a graph representation of the vertices
	demand_map_t m_demand_map;
	vector<vesselclass> m_fleet;
	map<string, vertex_descriptor> m_UNLOCODE_to_vertex;
	data_map_t m_instance_dist;
	demand_idx_map_t m_instance_dem;
	port_map_t m_instance_ports;
	Graph m_instance_graph;

	vertex_descriptor UNLOCODE_to_vertex ( string UNLOCODE );
	//vesselclass get_vessel_class(int capacity);
	void test_bundled_properties();

	pair<double, double> m_suez;

	pair<double, double> m_panama;

	//vector<rotation> m_rotations; //rotation variables for validator
};
}
#endif
