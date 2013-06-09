#pragma once
#include <vector>
#include <map>


typedef std::pair<int, int> coord;

struct cell
  {
  cell(const int& xx = -1, const int& yy = -1);
  coord xy;
  double north, south, east, west; // the cost to go in that direction.
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

class Map // A Map has edges that connect to each other west-east. North and south and impassable.
  {
  public:
    Map(int width1, int height1);
    cell& ref(const int& x, const int& y);
    std::vector<coord> pathfind(const int& x1, const int& y1, const int& x2, const int& y2); // empty = fail

  private:
    std::vector<cell> grid;
    double heuristic(const coord& xy1, const coord& xy2);
    double costTo(const coord& c1, const coord& c2);
    node& findLowestF(std::map<coord, node>& input);
    std::vector<cell> findNeighborList(const coord& current);
    std::vector<coord> reconstructPath(std::map<coord, node> paths, const coord& dest);
    cell null;
    const int width;
    const int height;
  };