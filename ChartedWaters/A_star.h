#pragma once
#include "worldMap.h"
#include <map>
#include <vector>
//#include <libtcod.hpp> /// testing only.

struct cell
  {
  cell(const int& xx = -1, const int& yy = -1);
  coord xy;
  double shallowCost;
  double deepCost;
  bool accessible;
  bool null;
  };

struct node
  {
  node();
  node(const coord& crd);
  node(const coord& crd, const double& gscore, const double& fscore, bool isStart = false);
  coord position;
  coord cameFrom;
  double g_score, f_score;
  bool start;
  };

class HeapMap
  {
  public:
    node findLowestF();
    //node& refOpenset(const coord& cor);
    //node& refClosedset(const coord& cor);
    void insertOpen(const coord& key, const node& val);
    void insertClosed(const coord& key, const node& val);
    bool isInOpenset(const coord& key);
    bool isInClosedset(const coord& key);
    node findClosed(const coord& key);
    void removeClosed(const coord& key); // if exists
    void removeOpen(const coord& key);
    int openmapSize();
    std::map<coord, node>& refToClosedSet();

  private:
    
    std::vector<std::pair<double, node>> heap;
    std::map<coord, node> open;
    std::map<coord, node> closed;
  };


class PathMap
  {
  friend class Pather;
  public:
    PathMap();
  PathMap(WorldMapClass& wmc); // Create a pathmap representation of World Map.
  cell& ref(const int& x, const int& y);
  cell& ref(const coord& xy);
  std::vector<cell> findNeighborList(const coord& current);
  std::pair<int,int> getDimensions();

  private:
    cell null;
    const int width;
    const int height;
    std::vector<cell> grid;
  };

class Pather
  {
  public:
    Pather();
    Pather(WorldMapClass& wmc);
    std::vector<coord> path(const coord& starting, const coord& destination, const double waveResistance); // Each path must not change the internal state.
    PathMap map; // ref to the map it paths on.

    void stepFloodfill();
    void completeFloodfill();
   // TCODConsole* console;
    std::vector<coord> getFillNeighbors(coord current);
    std::map<coord, bool> fill_openset;
    std::map<coord, int> fill_closedset; // 0 for explored, 1 for starting point.
    coord start;
    bool done;
    bool metOrigin;
    bool metTop, metBot, metLeft;

  private:
    
    double heuristic(const coord& xy1, const coord& xy2);
    double costTo(const coord& c2, const int& waveResistance);
    node& findLowestF(std::map<coord, node>& input);
    std::vector<coord> reconstructPath(std::map<coord, node> paths, const coord& dest);
  };