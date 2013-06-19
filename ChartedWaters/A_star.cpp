#include "A_star.h"
#include <cmath>
#include <iostream>

using namespace std;

cell::cell(const int& xx, const int& yy)
  : xy(coord(xx, yy)), shallowCost(0), deepCost(0), accessible(true), null(false)
  {

  }

node::node()
  : position(coord(-1,-1)), cameFrom(coord(-1,-1)), g_score(0), f_score(0), start(false)
  {

  }

node::node(const coord& crd)
  : position(crd), cameFrom(coord(-1,-1)), g_score(0), f_score(0), start(false)
  {

  }

node::node(const coord& crd, const double& gscore, const double& fscore, bool isStart)
  : position(crd), g_score(gscore), f_score(fscore), start(isStart)
  {

  }

PathMap::PathMap(WorldMapClass& wmc)
  :width(wmc.getWidth()), height(wmc.getHeight())
  {
  null.null = true;
  grid.reserve(sizeof(cell) * width * height + 1);

  
  for (int ycounter = 0; ycounter < height; ycounter++)
    for (int xcounter = 0; xcounter < width; xcounter++)
      {
      cell buffer;
      buffer.xy = coord(xcounter, ycounter);
      auto it = wmc.ref(xcounter, ycounter);
      if (it.altitude > 0 && !it.isCoastal) //If it is a inland tile
        buffer.accessible = false;
      else if (it.isCoastal) // If coastal
        buffer.shallowCost = 15;
      else if (it.altitude > -3 && it.altitude <= 0) // shallow
        buffer.shallowCost = 1 + abs(it.altitude) / 2;
      else if (it.altitude <= -3)
        buffer.deepCost = abs(it.altitude) / 10.0f;

      grid.push_back(buffer);
      }
  }

cell& PathMap::ref(const int& x, const int& y)
  {
  if (y >= 0 && y < height)
    {
    if (x >= 0 && x < width)
      return grid[x + y * width];
    else if (x < 0)
      return grid[width + x % width + y * width];
    else
      return grid[x % width + y * width];
    }
  else
    return null;
  }

std::vector<cell> PathMap::findNeighborList(const coord& current)
  {
  vector<cell> output;
  if (!ref(current).accessible) // First, if the tile I am in isn't accessible, there are no neighbors.
    return output;
  // Check all eight directions.
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

coord PathMap::getDimensions()
  {
  return coord(width, height);
  }

Pather::Pather(WorldMapClass& wmc)
  : map(wmc)
  {

  }

cell& PathMap::ref(const coord& xy)
  {
  return ref(xy.first, xy.second);
  }

vector<coord> Pather::path(const coord& starting, const coord& destination, const double waveResistance)
  {
  if (!map.ref(destination).accessible)
    {
    return vector<coord>(2, starting);
    }

  std::map<coord, node> closedset;
  std::map<coord, node> openset;

  coord start = starting;
  coord dest = destination;
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
    neighbors = map.findNeighborList(current.position);
    for (auto it = neighbors.begin(); it < neighbors.end(); it++)
      {
      double tentative_g = current.g_score + costTo(it->xy, waveResistance);
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

  vector<coord> retval(2, starting);
    return retval; // means the path stays in place.
  }

double Pather::costTo(const coord& c2, const int& waveResistance)
  {
  double cost = 0;
  cost += map.ref(c2).shallowCost;
  cost += map.ref(c2).deepCost * (double)(16 - waveResistance) / 16;
  //cost = 1;

  return cost;
  }

double Pather::heuristic(const coord& xy1, const coord& xy2)
  {
  const double scalingFactor = 1; //// making this larger can make the pathfinding faster, at the cost of accuracy

  double dY = xy1.second - xy2.second;
  dY *= dY; // dY^2

  // Now, for dX...
  // first, "normal" distance.
  double XNormal = xy1.first - xy2.first;
  XNormal = abs(XNormal);
  //Then edge distance.
  double XEdge = map.width - XNormal; // 0, x1, x2, edge. 

  XEdge *= XEdge;
  XNormal *= XNormal;

  double H = XNormal > XEdge ? XEdge : XNormal; // Take shorter.
  H += dY;
  H = sqrt(H);
  
  return H * scalingFactor * (1.001f); // 1.001f is an optimizing tie-breaker.
  }

node& Pather::findLowestF(std::map<coord, node>& input)
  {
  auto it = input.begin();
  for (auto it2 = input.begin(); it2 != input.end(); it2++)
    {
    if ( it->second.f_score > it2->second.f_score )
      it = it2;
    }
  return it->second;
  }

std::vector<coord> Pather::reconstructPath(std::map<coord, node> paths, const coord& dest)
  {
  vector<coord> output;
  if (paths.find(dest) != paths.end() && paths.find(dest)->second.cameFrom != dest ) // if in the map and it doesn't lead to itself
    {
    cout << "r";
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