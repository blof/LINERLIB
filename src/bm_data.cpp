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

#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator

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


//#include "../include/BM_Graph.hpp"
#include "../include/bm_data.hpp"

#define DEBUG_OUT

using namespace std;
using namespace boost;

typedef std::map < std::pair<graph_traits<BM::Graph>::vertex_descriptor, graph_traits<BM::Graph>::vertex_descriptor> , boost::tuple<double, double, double> > demand_map_t;
typedef boost::graph_traits < BM::Graph >::vertex_descriptor vertex;
typedef boost::graph_traits < BM::Graph >::edge_descriptor edge;



void BM::data::print_demands()
{

  demand_map_t::iterator it, itend;
  itend=m_demands.end();
  for ( it=m_demands.begin(); it!=itend; it++ )
    {
      pair<vertex_descriptor, vertex_descriptor> key=it->first;
      boost::tuple<double, double, double> value =it->second;
      cout << m_graph[key.first].UNLOCODE << "-" << m_graph[key.second].UNLOCODE << " FFE: " <<
           get<0> ( value ) << " $: " << get<1> ( value ) << " TT: " << get<2> ( value ) << endl;
    }
}

void BM::data::print_demands_by_id()
{
string unl_1, unl_2;
  demand_idx_map_t::iterator it, itend;
  port_map_t::iterator p_it, p_itend;
  itend=m_demands_by_id.end();
  p_itend=m_ports.end();
  for ( it=m_demands_by_id.begin(); it!=itend; it++ )
    {
      pair<int, int> key=it->first;
      boost::tuple<double, double, double> value =it->second;
      p_it=m_ports.find(key.first);
      if(p_it!=p_itend) unl_1=p_it->second;
      p_it=m_ports.find(key.second);
      if(p_it!=p_itend) unl_2=p_it->second;
      cout << unl_1 << "-" << unl_2 << " FFE: " <<
      get<0> ( value ) << " $: " << get<1> ( value ) << " TT: " << get<2> ( value ) << endl;
    }
}

void BM::data::print_distances_by_id()
{
string unl_1, unl_2;
  data_map_t::iterator it, itend;
  port_map_t::iterator p_it, p_itend;
  itend=m_distances_by_id.end();
  p_itend=m_ports.end();
  for ( it=m_distances_by_id.begin(); it!=itend; it++ )
    {
      pair<int, int> key=it->first;
      boost::tuple<double, bool, bool> value =it->second;
      p_it=m_ports.find(key.first);
      if(p_it!=p_itend) unl_1=p_it->second;
      p_it=m_ports.find(key.second);
      if(p_it!=p_itend) unl_2=p_it->second;
      cout << unl_1 << "-" << unl_2 << " nm: " <<
      get<0> ( value ) << " is suez: " << get<1> ( value ) << " is panama: " << get<2> ( value ) << endl;
    }
}



void BM::data::print_graph ( string filename )
{
  node_writer nw ( m_graph );
  edge_writer ew ( m_graph );
  std::ofstream of ( filename.c_str() );
  write_graphviz ( of, m_graph,nw, ew );
}

void BM::data::print_inst_graph ( string filename )
{
  node_writer nw ( m_instance_graph );
  edge_writer ew ( m_instance_graph );
  std::ofstream of ( filename.c_str() );
  write_graphviz ( of, m_instance_graph,nw, ew );
}

void BM::data::print_fleet()
{
  vector<vesselclass>::iterator it, it_end;
  it_end=m_fleet.end();
  for ( it=m_fleet.begin(); it != it_end; it++ )
    {
      cout << "Name: " <<  it->m_name << " FFE: " <<  it->m_capacity << " OPEX: " <<  it->m_OPEX_cost <<" design speed: " <<   it->m_design_speed << " fuel consumption " <<  it->m_fuel_consumption <<" idle consumption " <<  it->m_idle_consumption <<" quantity " << it->m_quantity << " suez cost " << it->m_suez_cost << " panama cost " << it->m_panama_cost << endl;
    }
}

void BM::data::print_ports()
{
  port_map_t::iterator it, itend;
  itend=m_ports.end();
  for ( it=m_ports.begin(); it!=itend; it++ )
    {
      cout << it->first << "-" << it->second<< endl;
    }
}
