/*
   Data object for benchmark data
    Copyright (C) <2011>  <Berit Løfstedt & C.E.M. Plum, DTU & Maerskline>

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

#ifndef BM_DATA_HPP
#define BM_DATA_HPP

#include "../include/BM_Graph.hpp"
#include "bm_rotation.h"

#include <string>
#ifdef WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

//Hash_functions for unordered maps for the key pair<int, int>
namespace std
  {
#ifndef VS2010
  namespace tr1
    {
#endif
		template<>
    struct hash< pair< int, int> >
      {
        std::size_t operator() ( pair<int, int> val ) const
          {
            return hash<int>() ( val.first ) + hash<int>() ( val.second );
          }
      };
#ifndef VS2010
  }
#endif
}

/*********************************
Variables
**********************************/
//Demands
typedef std::map < std::pair<graph_traits<BM::Graph>::vertex_descriptor, graph_traits<BM::Graph>::vertex_descriptor> , boost::tuple<double, double, double> > demand_map_t;
typedef std::map < std::pair<int, int> , boost::tuple<double, double, double> > demand_idx_map_t;
//typedef std::tr1::unordered_map<std::pair<int, int>,boost::tuple<double, bool, bool> > data_map_t;
//typedef std::tr1::unordered_map<std::pair<int, int>, std::string> iistrmap_t;
//typedef std::tr1::unordered_map<int, string> port_map_t;
typedef std::multimap<std::pair<int, int>,boost::tuple<double, bool, bool> > data_map_t;
typedef std::map<int, string> port_map_t;

typedef std::map<std::pair<int, int>, std::string> iistrmap_t;



namespace BM
  {
  struct vesselclass
    {
      //Name, Cap, TC, Draft, Min s, max s, s* ,bunker, suez cost, panama cost
      vesselclass ( int id = 0, string name= "", FFE capacity = 0,  integer OPEXCost= 0, double draft = 0.0, double min_speed = 0.0, double max_speed = 0.0, double d_speed = 0.0,
                    double fuelConsumption= 0.0, double idleConsumption= 0.0, double panama=0, double suez=0 ) : m_idx ( id ), m_name ( name ), m_capacity ( capacity ),
          m_draft ( draft ) , m_design_speed ( d_speed ), m_min_speed ( min_speed ), m_max_speed ( max_speed ), m_fuel_consumption ( fuelConsumption ),m_idle_consumption ( idleConsumption ),
          m_OPEX_cost ( OPEXCost ), m_panama_cost(panama), m_suez_cost(suez)  {
		  m_quantity=0;
	}

          void set_qnt(int q){
		  m_quantity=q;
	  }
	  
      uint m_idx; // The index of the vesselClass
      string m_name; // The name of the vessel Class
      FFE m_capacity; // The maximal load for the vesseltype
      double m_draft; // The average draft of the vesselclass
      double m_design_speed; // The design speed of the vesselclass, in knots: NM/Hr
      double m_min_speed; // The minimum speed of the vesselclass, in knots: NM/Hr
      double m_max_speed; // The maximal speed of the vesselclass, in knots: NM/Hr
      double m_fuel_consumption; // The fuel consumption in metric tonnes per day sailing at design speed, for the vessel class
      double m_idle_consumption; // The fuel consumption in metric tonnes per day idling at port, for the vessel class
      integer m_OPEX_cost; // The cost to have the vessel sailing a single day
      int m_quantity;
      double m_panama_cost;
      double m_suez_cost;
    };

  class data
    {
    public:
      data() {    };
      //Constructor containing complete graph regardless of instance size
      data ( bool all_to_all, Graph G, vector<vesselclass> fleet, demand_map_t demand_map ) : m_all_to_all_graph ( all_to_all ), m_graph ( G ), m_fleet ( fleet ), m_demands ( demand_map ) {};
      //Constructor containing complete graph and graph and data structures specific to the instance
      data ( bool all_to_all, Graph G, vector<vesselclass> fleet, demand_map_t demand_map, port_map_t ports, Graph inst_graph, demand_idx_map_t demand_id_map, data_map_t distance_map_ids, pair<double,double> suez , pair<double,double> panama ) :
          m_all_to_all_graph ( all_to_all ), m_graph ( G ), m_fleet ( fleet ), m_demands ( demand_map ), m_ports ( ports ), m_instance_graph ( inst_graph ), m_demands_by_id ( demand_id_map ), m_distances_by_id ( distance_map_ids ), m_suez_cost_factors(suez), m_panama_cost_factors(panama) {};

      void set_graph ( Graph G )
      {
        m_graph=G;
      }
      void set_fleet ( vector<vesselclass> fleet )
      {
        m_fleet= fleet;
      }
      void  set_demands ( demand_map_t demands )
      {
        m_demands=demands;
      }

      const Graph get_graph()
      {
        return m_graph;
      }

      const Graph& get_instance_graph()
      {
        return m_instance_graph;
      }

      const vector<vesselclass> get_fleet()
      {
        return m_fleet;
      }

      /**returns a demand map with key indicating vertex_descriptors in original graph (m_graph)
      */
      const demand_map_t get_demands()
      {
        return m_demands;
      }

      /**@returns a demand map with key consisting of port idxs
      */
      const demand_idx_map_t get_demands_by_id()
      {
        return m_demands_by_id;
      }
      
 

      const bool get_all_to_all_graph()
      {
        return m_all_to_all_graph;
      }

      void set_distances ( data_map_t dist )
      {
        m_distances_by_id=dist;
      }

      void set_ports ( port_map_t ports )
      {
        m_ports=ports;
      }

      const data_map_t get_distances()
      {
        return m_distances_by_id;
      }

      const port_map_t get_ports()
      {
        return m_ports;
      }
      
      pair<double, double > get_suez_cost_factors(){
	return m_suez_cost_factors;
      }

      pair<double, double > get_panama_cost_factors(){
	return m_panama_cost_factors;
      }
      
      void print_graph ( string filename );
      void print_inst_graph ( string filename );
      void print_demands();
      void print_demands_by_id();
      void print_fleet();
      void print_ports();
      void print_distances_by_id();

    private:
      //Graph can either be a sparse graph, including waypoints, with distances in between these and ports. Or a complete graph with all port-port Distances, but no waypoints.
      bool m_all_to_all_graph;

      Graph m_graph;

      //vector of all vessel classes
      vector<vesselclass> m_fleet;

      //Demands mapped as vertex descriptors to the original graph of all ports (m_graph)
      demand_map_t m_demands;

      //the ports included in the instance defined by the demands
      port_map_t m_ports;

      //the instance graph is a complete graph consisting only of the vertices included in this benchmark instance
      Graph m_instance_graph;

      //Demands mapped to the port_id of the origin and destination nodes
      demand_idx_map_t m_demands_by_id;

      //The distances as a function of port_id, port_id
      data_map_t m_distances_by_id;
      
      pair<double, double> m_suez_cost_factors;
      
      pair<double, double> m_panama_cost_factors;

    };
}
#endif // BM_DATA_HPP
