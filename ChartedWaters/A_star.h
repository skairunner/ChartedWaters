#pragma once
#include "worldMap.h"
#include <map>
#include <vector>


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

class PathMap
  {
  friend class Pather;
  public:
    PathMap();
  PathMap(WorldMapClass& wmc); // Create a pathmap representation of World Map.
  cell& ref(const int& x, const int& y);
  cell& ref(const coord& xy);
  std::vector<cell> findNeighborList(const coord& current);

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
    Pather(PathMap& mmap);
    std::vector<coord> path(const coord& starting, const coord& destination, const double waveResistance); // Each path must not change the internal state.
    
  private:
    PathMap& map; // ref to the map it paths on.
    double heuristic(const coord& xy1, const coord& xy2);
    double costTo(const coord& c2, const int& waveResistance);
    node& findLowestF(std::map<coord, node>& input);
    std::vector<coord> reconstructPath(std::map<coord, node> paths, const coord& dest);
  };