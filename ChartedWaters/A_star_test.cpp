#include "A_star_test.h"
#include <cmath>
#include <iostream>

using namespace std;

cell::cell(const int& xx, const int& yy)
  : xy(coord(xx, yy)), null(false), accessible(true), north(1), south(1), east(1), west(1)
  {}

node::node()
  : position(coord(-1, -1)), cameFrom(coord(-1, -1)), g_score(0), f_score(0), start(false)
  {}

node::node(const coord& crd)
  : position(crd), cameFrom(coord(-1, -1)), g_score(0), f_score(0), start(false)
  {}

node::node(const coord& crd, const double& gscore, const double& fscore, bool isStart)
  : position(crd), cameFrom(coord(-1,-1)), g_score(gscore), f_score(fscore), start(isStart)
  {}

Map::Map(int width1, int height1)
  : width(width1), height(height1)
  {
  grid.reserve(sizeof(cell) * width * height);
  for (int ycounter = 0; ycounter < height; ycounter++)
    for (int xcounter = 0; xcounter < width; xcounter++)
      grid.push_back(cell(xcounter, ycounter));
  null.null = true;
  }

double Map::heuristic(const coord& xy1, const coord& xy2)
  {
  // Currently using the diagonal shortcut algorithm :v
  const double scalingFactor = 1; //// making this larger can make the pathfinding faster, at the cost of
  // loss of accuracy. Maximum error is at most * scale.
  /*double xDist = abs(xy1.first - xy2.first);
  double yDist = abs(xy1.second - xy2.second);
  double H;
  if (xDist > yDist)
    H = 14 * yDist + 10 * (xDist - yDist);
  else
    H = 14 * xDist + 10 * (yDist - xDist);*/
  double H = sqrt(pow(xy1.first - xy2.first,(double)2) + pow((double)(xy1.second, xy2.second), 2));

  return H * scalingFactor;
  }

double Map::costTo(const coord& c1, const coord& c2)
  {
  double xcost = 0;
  double ycost = 0;
  if (c1 == c2)
    return 0;
  int xDist = c2.first - c1.first;
  int yDist = c2.second - c1.second;
  if (xDist == -1)
    xcost = ref(c1.first, c2.second).west;
  else if (xDist == 1)
    xcost = ref(c1.first, c2.second).east;
  else if (xDist == 0)
    ;
  else
    cerr << "Something wrong happened in the cost function\n";

  if (yDist == -1)
    ycost = ref(c1.first, c2.second).north;
  else if (yDist == 1)
    ycost = ref(c1.first, c2.second).south;
  else if (yDist == 0)
    ;
  else
    cerr << "Something wrong happened in the cost function\n";

  double cost = sqrt(xcost * xcost + ycost * ycost);

  return cost;    
  }

vector<cell> Map::findNeighborList(const coord& current)
  {
  // Check all eight directions.
  vector<cell> output;
  // topleft
  auto it = ref(current.first - 1, current.second - 1);
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref(current.first, current.second - 1); // top
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref (current.first +1 , current.second - 1); // topright
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref(current.first - 1, current.second); // left
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref(current.first + 1, current.second); //right
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref(current.first - 1, current.second +1); // bottom left
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref(current.first, current.second + 1); // bottom
  if (!it.null && it.accessible)
    output.push_back(it);

  it = ref(current.first + 1, current.second + 1); // bottom right
  if (!it.null && it.accessible)
    output.push_back(it);

  return output;
  }

node& Map::findLowestF(std::map<coord, node>& input)
  {
  auto it = input.begin();
  for (auto it2 = input.begin(); it2 != input.end(); it2++)
    {
    if ( it->second.f_score > it2->second.f_score )
      it = it2;
    }
  return it->second;
  }

cell& Map::ref(const int& x, const int& y)
  {
  if (x >= 0 && x < width && y >= 0 && y < height)
    return grid[x + y * width];
  else
    return null;
  }

vector<coord> Map::pathfind(const int& x1, const int& y1, const int& x2, const int& y2)
  {
  coord start(x1, y1);
  coord dest(x2, y2);
  map<coord, node> closedset;
  map<coord, node> openset;
  openset[start] = node(start, 0, heuristic(start, dest), true);

  vector<cell> neighbors;
  neighbors.clear();
  while (openset.size() != 0)
    {
    cout << "e";
    node current = findLowestF(openset);
    // Push current onto closed, and pop current from open.
    closedset[current.position] = current;

    if (current.position == dest) // if we've arrived!
      return reconstructPath(closedset, dest);

    openset.erase(current.position);
    neighbors = findNeighborList(current.position);
    for (auto it = neighbors.begin(); it < neighbors.end(); it++)
      {
      double tentative_g = current.g_score + costTo(current.position, it->xy);
      // if it's in the closedset, AND (since it exists there should be a value) the existing value is cheaper.
      auto neighborInClosedSet = closedset.find(it->xy);
      if (neighborInClosedSet != closedset.end() && tentative_g >= neighborInClosedSet->second.g_score)
        continue; // skip
      // if the neighbor isn't in openset, or the tentative g is cheaper
      if (openset.find(it->xy) == openset.end() || (neighborInClosedSet != closedset.end() && tentative_g < neighborInClosedSet->second.g_score))
        {
        auto InClosedSet = closedset.find(it->xy);
        if (InClosedSet != closedset.end()) // if in the closed set
          closedset.erase(InClosedSet);

        // neighborInOpenSet
       // auto InOpenSet = openset[it->xy]; 
        node InOpenSet;
        InOpenSet.position = it->xy;
        InOpenSet.cameFrom = current.position;
        InOpenSet.g_score = tentative_g;
        InOpenSet.f_score = InOpenSet.g_score + heuristic(InOpenSet.position, dest);
        openset[it->xy] = InOpenSet; // This implicitly adds this neighbor to openset if it wasn't there before.
        }
      }
    }
  // Since we exited the loop with nothing to show for it, you fail.
  return vector<coord>(); // empty = fail
  }

vector<coord> Map::reconstructPath(map<coord, node> paths, const coord& dest)
  {
  vector<coord> output;
  if (paths.find(dest) != paths.end()) // if in the map
    {
    auto it = reconstructPath(paths, paths.find(dest)->second.cameFrom);
    output.reserve(output.size() + it.size());
    output.insert(output.end(), it.begin(), it.end());
    output.push_back(dest);
    return output;
    }
  else
    {
    output.push_back(dest);
    return output;
    }
  }