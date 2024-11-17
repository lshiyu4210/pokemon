// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include "pokemon.h"
#include <iomanip>

//Global functions
void Pokemon::read_vertices() {
    cin >> V;
    if(mode == Mode::MST) {
        plane.resize(V);
        for (uint32_t i = 0; i < V; ++i) {
            int x, y;
            Type t;
            cin >> x >> y;
            if (x < 0 && y < 0) t = Type::SEA;
            else if ((x == 0 && y <= 0) || (y == 0 && x <= 0))  t = Type::COAST;
            else                t = Type::LAND;
            plane[i] = {x, y, t};
        }
    }
    else {
        grid.resize(V);
        for (uint32_t i = 0; i < V; ++i) {
            int x, y;
            cin >> x >> y;
            grid[i] = {x, y};
        }
    }
}

void Pokemon::output() {
    cout << fixed << showpoint << setprecision(2);
    if (mode == Mode::MST) {
        cout << total_weight << "\n";
        for(uint32_t i = 1 ; i < Prims_table.size(); ++i)  {
            if (Prims_table[i].parent > i)   cout << i << " " << Prims_table[i].parent << "\n";
            else                            cout << Prims_table[i].parent << " " << i << "\n";
        }
    }
    else if (mode == Mode::FASTTSP) {
        cout << total_weight << "\n";
        for(uint32_t i = 0 ; i < currPath.size(); ++i)  {
            cout << currPath[i] << " ";
        }
        cout << "\n";
    }
    else {
        cout << bestCost << "\n";
        for(uint32_t i = 0 ; i < bestPath.size(); ++i)  {
            cout << bestPath[i] << " ";
        }
        cout << "\n";
    }
}

//Implementations for Part A
void Pokemon::Prims_MST() {
    Prims_table.resize(V);
    Prims_table[0].dist = 0;
    for (uint32_t i = 0; i < V; ++i) {
        uint32_t v = UINT32_MAX;
        for (uint32_t j = 0; j < V; ++j) {
            if (!Prims_table[j].visited && (v == UINT32_MAX || Prims_table[j].dist < Prims_table[v].dist))
                v = j;
        }
        if (Prims_table[v].dist == INF) {
            cerr << "Cannot construct MST\n";
            exit(1);
        }
        Prims_table[v].visited = true;
        if(v != 0) {
            Prims_table[v].dist = sqrt(Prims_table[v].dist);
            total_weight += Prims_table[v].dist;
        }
        for (uint32_t j = 0; j < V; ++j) {
            if(j != v && !Prims_table[j].visited) {
                double distance = squared_distance(plane[v], plane[j]);
                if (distance < Prims_table[j].dist) {
                    Prims_table[j].dist = distance;        
                    Prims_table[j].parent = v;
                }
            }
        }

    }
}

double Pokemon::squared_distance(Coordinate &c1, Coordinate &c2) {
    if((c1.type == Type::LAND && c2.type == Type::SEA) || (c2.type == Type::LAND && c1.type == Type::SEA))
                return INF;
    else        return static_cast<double>(c1.x - c2.x) * (c1.x - c2.x) + static_cast<double>(c1.y - c2.y) * (c1.y - c2.y);
}



//Implementation for Part B
double Pokemon::squared_distance(Point &p1, Point &p2) {
    return static_cast<double>(p1.x - p2.x) * (p1.x - p2.x) + static_cast<double>(p1.y - p2.y) * (p1.y - p2.y);
}

double Pokemon::euc_dis(Point &p1, Point &p2) {
    return sqrt(squared_distance(p1, p2));
}

uint32_t Pokemon::search(uint32_t p) {
    uint32_t index = 0;
    double min_dist = INF;
    for (uint32_t i = 0; i < grid.size(); ++i) {
        if (!grid[i].visited) {
            double d = squared_distance(grid[p], grid[i]);
            if (d < min_dist) {
                index = i;
                min_dist = d;
            }
        }
    }
    return index;
}

void Pokemon::nearest_neighbor() {
    currPath.reserve(V);
    currPath.push_back(0);
    grid[0].visited = true;
    for (uint32_t i = 0; i < grid.size() - 1; ++i) {
        uint32_t last = currPath.back();
        uint32_t next = search(last);
        currPath.push_back(next);
        grid[next].visited = true;
    }
}

void Pokemon::two_opt() {
    for (uint32_t i = 0; i < currPath.size() - 2; ++i) {
        for (uint32_t j = i + 2; j < currPath.size(); ++j) {
            double d1 = 0;
            double d2 = 0;
            if (j + 1 != currPath.size()) {
                d1 = squared_distance(grid[currPath[i]], grid[currPath[j]]) + squared_distance(grid[currPath[i + 1]], grid[currPath[j + 1]]);
                d2 = squared_distance(grid[currPath[i]], grid[currPath[i + 1]]) + squared_distance(grid[currPath[j]], grid[currPath[j + 1]]);
            }
            else {
                d1 = squared_distance(grid[currPath[i]], grid[currPath[j]]) + squared_distance(grid[currPath[i + 1]], grid[currPath[0]]);
                d2 = squared_distance(grid[currPath[i]], grid[currPath[i + 1]]) + squared_distance(grid[currPath[j]], grid[currPath[0]]);
            }
            if (d1 < d2) {
                reverse(currPath.begin() + i + 1, currPath.begin() + j + 1);
            }
        }
    }
}

void Pokemon::random_insertion() {
    currPath.reserve(0);
    currPath.push_back(0); // select a random starting city

    // iterate over remaining grid
    for(uint32_t i = 1; i < V; i++) {
        uint32_t minPos = UINT32_MAX;
        double minDist = INF;
        size_t s = currPath.size();
        // iterate over currPath to find the best position to insert the city
        for(uint32_t j = 0; j < s; j++) {
            double d = 0;
            if(j + 1 != s) {
                d = euc_dis(grid[currPath[j]], grid[i]) + euc_dis(grid[i], grid[currPath[(j + 1)]]) -
                       euc_dis(grid[currPath[j]], grid[currPath[(j + 1)]]);
            }
            else {
                d = euc_dis(grid[currPath[j]], grid[i]) + euc_dis(grid[i], grid[currPath[(0)]]) -
                       euc_dis(grid[currPath[j]], grid[currPath[(0)]]);
            }
            if(d < minDist) {
                minDist = d;
                minPos = j;
            }
        }

        // insert the city at the best position
        currPath.insert(currPath.begin() + (minPos + 1), i);
    }
}

void Pokemon::fast_tsp() {
    // nearest_neighbor();
    // two_opt();
    random_insertion();
    for (uint32_t i = 0; i < currPath.size() - 1; ++i) {
        total_weight += euc_dis(grid[currPath[i]], grid[currPath[i + 1]]);
    }
    total_weight += euc_dis(grid[currPath[0]], grid[currPath[V - 1]]);
}




//Implementation for Part C
void Pokemon::update() {
    if(currCost < bestCost) {
        bestCost = currCost;
        bestPath = currPath;
    }
    // cout << currCost << "\n";
    // for(uint32_t i = 0 ; i < currPath.size(); ++i)  {
    //     cout << currPath[i] << " ";
    // }
    // cout << "\n";
}

double Pokemon::modified_MST(vector<uint32_t> &rest) {

    //Some vertices not all
    //calculate without printing
    //Euclidean distance
    double total_cost = 0;
    uint32_t n = (uint32_t)rest.size();
    Prims_table.resize(n);
    Prims_table[0].dist = 0;
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t v = UINT32_MAX;
        for (uint32_t j = 0; j < n; ++j) {
            if (!Prims_table[j].visited && (v == UINT32_MAX || Prims_table[j].dist < Prims_table[v].dist))
                v = j;
        }
        if (Prims_table[v].dist == INF) {
            cerr << "Cannot construct MST\n";
            exit(1);
        }
        Prims_table[v].visited = true;
        if(v != 0) {
            Prims_table[v].dist = sqrt(Prims_table[v].dist);
            total_cost += Prims_table[v].dist;
        }
        for (uint32_t j = 0; j < n; ++j) {
            if(j != v && !Prims_table[j].visited) {
                double distance = squared_distance(grid[rest[v]], grid[rest[j]]);
                if (distance < Prims_table[j].dist) {
                    Prims_table[j].dist = distance;        
                    Prims_table[j].parent = v;
                }
            }
        }
    }
    Prims_table.clear();
    return total_cost;
}

bool Pokemon::promising(uint32_t permLength) {
    uint32_t k = V - permLength;
    if (k == 4)    return true;
    double estimate = currCost;
    vector<uint32_t> rest(k);
    for (uint32_t i = 0; i < k; ++i) {
        rest[i] = currPath[i + permLength];
    }
    double mst  = modified_MST(rest);
    estimate += mst;
    double min_front = INF;
    double min_back = INF;
    for (uint32_t i = 0; i < k; ++i) {
        uint32_t temp = rest[i];
        double front_dist = squared_distance(grid[0], grid[temp]);
        double back_dist = squared_distance(grid[currPath[permLength - 1]], grid[temp]);
        if (front_dist < min_front)     min_front = front_dist;
        if (back_dist < min_back)     min_back = back_dist;
    }
    estimate += (sqrt(min_front) + sqrt(min_back));

    //debug
    // cout << fixed << showpoint << setprecision(2);
    // for(uint32_t i = 0 ; i < currPath.size(); ++i)  {
    //     cout << currPath[i] << " ";
    // }
    // cout << "\t" << permLength << "\t" << currCost << "\t" << sqrt(min_front) << "\t" << sqrt(min_back) << "\t" << mst;
    // cout << "\t" << estimate << "\t";
    // if (estimate < bestCost) {
    //     cout << "true\n";
    // }
    // else    cout << "false\n";

    if (estimate < bestCost)    return true;
    return false;
}

void Pokemon::genPerms(uint32_t permLength) {
  if (permLength == currPath.size()) {
    double closingEdge = sqrt(squared_distance(grid[0], grid[currPath[permLength - 1]]));
    currCost += closingEdge;
    update();
    currCost -= closingEdge;
    return;
  }  // if ..complete path

  if (!promising(permLength)) {
    return;
  }  // if ..not promising

  for (uint32_t i = permLength; i < currPath.size(); ++i) {
    swap(currPath[permLength], currPath[i]);
    double currEdge = sqrt(squared_distance(grid[currPath[permLength - 1]], grid[currPath[permLength]]));
    currCost += currEdge;
    genPerms(permLength + 1);
    currCost -= currEdge;
    swap(currPath[permLength], currPath[i]);
  }  // for ..unpermuted elements
}  // genPerms()

void Pokemon::opt_tsp() {
    
    fast_tsp();
    bestCost = total_weight;
    bestPath = currPath;
    // currPath.clear();
    // currPath.resize(V);
    // currPath[0] = 0;
    genPerms(1);
}

