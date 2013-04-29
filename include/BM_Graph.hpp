#ifndef BM_GRAPH_HPP
#define BM_GRAPH_HPP

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <iostream>

using namespace boost;
using namespace std;


/*********************************
Defines and Structs
**********************************/
typedef unsigned int portindex;
typedef unsigned int edgeindex;
//typedef unsigned int region;
//typedef unsigned int country;
//typedef unsigned int cabotage;
typedef unsigned long long FFE; //Number of Forty Foot Equivalent containers
typedef long long int integer;
typedef unsigned int uint;


namespace BM{
struct Node //Ports, Canals and Waypoints
  {

    //constructor for parsing:   UNLOCODE;name;country;cabotage;region;latitude;longitude;draft;moveCost;transhipmentCost;callCostFixed; callCostPerFFE
    Node ( int id = 0, string U = "", string N = "", string C = "", string CA="", string R= "",
              double lat=0.0, double lon= 0.0, double d=0.0,
              double t = 0.0, double l = 0.0 , double cf=0.0, double cffe=0.0 ) : idx ( id ), UNLOCODE ( U ), name ( N ), country ( C ), cabotage ( CA ), region ( R ),
        latitude ( lat ), longitude ( lon ), draft ( d ),
        transhipmentCost ( t ),localMoveCost ( l ), callCostFixed ( cf ), callCostPerFFE ( cffe ) {}


    portindex idx; //< The port's index in the list
    //GEO info
    string UNLOCODE; //< 5 digit UNLOCODE
    string name; //< The name of the port
    string country; //< The region the port belongs to
    string cabotage; //< The region the port belongs to
    string region; //< The region the port belongs to
    double latitude; //<  The latitude of the port
    double longitude; //<  the longitude of the port

    //port constraints
    double draft; //< The draft of the port

    //Costs
    double transhipmentCost; //< The cost for a tranship move, i.e a transhipment requires two moves
    double localMoveCost; //< The cost to load / unload a container
    double callCostFixed; //< The cost for calling the port basic rate
    double callCostPerFFE; //< The cost for calling the port additional per ffe capacity of vessel

    //Misc
    bool isWayPoint; //<
    bool isCanal; //<
  };

struct Edge
  {

    //constructor for parsing
    Edge ( int id = 0, double dist = 0.0, double d = 0.0, bool s= false, bool p=false )
        : idx ( id ),  distance ( dist ), draft ( d ), isSuezTraversal ( s ), isPanamaTraversal ( p ) {}

    edgeindex idx; //< The edge's index in the list
    double distance; //< The edge's distance in Nm, Nautical Miles

    //Data for All-All graphs only
    double draft; //< The edge's maximal draft
    bool isSuezTraversal; //<Does edge traverse Suez Canal
    bool isPanamaTraversal; //<Does edge traverse Panama Canal
  };

//Graph
typedef adjacency_list<
vecS ,
vecS ,
bidirectionalS,
Node,
Edge>
Graph;

//descriptor typedefs
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor;


//iterator typedefs
typedef boost::graph_traits<Graph>::vertex_iterator Viterator;
typedef boost::graph_traits<Graph>::edge_iterator Eiterator;

typedef boost::graph_traits<Graph>::in_edge_iterator IEiterator;
typedef boost::graph_traits<Graph>::out_edge_iterator OEiterator;

class node_writer
{
public:
   node_writer( BM::Graph& g)
      : g(g)
   {}
   void operator()(std::ostream& out,
                   const BM::Graph::vertex_descriptor& v) const
   {
      out << "[label=\"" << g[v].UNLOCODE << "\"]";
   }
private:
   BM::Graph& g;
}; 

class edge_writer
{
public:
   edge_writer( BM::Graph& g)
      : g(g)
   {}
   void operator()(std::ostream& out,
                   const BM::Graph::edge_descriptor& e) const
   {
     Graph::vertex_descriptor s,t;
     s=source(e, g);
     t=target(e,g);
      out << "[label=\"" << g[s].UNLOCODE << "->" <<g[t].UNLOCODE <<":"<< g[e].distance << "\"]";
   }
private:
   BM::Graph& g;
}; 



}
#endif
