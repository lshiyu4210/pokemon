// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#ifndef POKEMON_H
#define POKEMON_H
#include <iostream>
#include <vector>
#include <string>
#include <queue> 
#include <limits>
#include <cmath>
#include <algorithm>

using namespace std;

const double INF =  numeric_limits<double>::infinity();

enum class Mode {
  NONE = 0,
  MST,
  FASTTSP,
  OPTTSP,
};

enum class Type : uint8_t {
  LAND,
  SEA,
  COAST,
};

class Pokemon {

private:

    //Part A: Prim's MST
    struct Coordinate {
      int x;
      int y;
      Type type;
    };

    struct Column {
        double dist = INF;
        uint32_t parent;
        bool visited = false;
    };
    vector<Coordinate> plane;
    vector<Column> Prims_table;

    //Part B: FASTTSP
    struct Point {
      int x;
      int y;
      bool visited = false;
    };
    vector<Point> grid;
    
    //Global
    double total_weight = 0;
    u_int32_t V = 0;
    Mode mode = Mode::NONE;
    vector<uint32_t> currPath;
    double currCost = 0;
    vector<uint32_t> bestPath;
    double bestCost = INF;

  public:

    //Global
    Pokemon () {}
    void read_vertices ();
    void get_mode(int argc, char** argv);
    void select_mode();
    void output();

    //Part A
    void Prims_MST();
    double squared_distance(Coordinate &c1, Coordinate &c2);

    //Part B
    double squared_distance(Point &p1, Point &p2);
    double euc_dis(Point &p1, Point &p2);
    uint32_t search(uint32_t p);
    void nearest_neighbor();
    void two_opt();
    void random_insertion();
    void fast_tsp();

    //Part C
    void update();
    double modified_MST(vector<uint32_t> &rest);
    bool promising(uint32_t permLength);
    void genPerms(uint32_t permLength);
    void opt_tsp();
    
    


};


#endif