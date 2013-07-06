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
//  console = new TCODConsole(map.width, map.height);
  done = false;
  metTop = metBot = metLeft = false;
  start = coord(0, 0);
  }

cell& PathMap::ref(const coord& xy)
  {
  return ref(xy.first, xy.second);
  }

vector<coord> Pather::path(const coord& starting, const coord& destination, const double waveResistance)
  {
#ifdef NDEBUG
  auto searchResult = fill_closedset.find(destination);
#endif
#ifndef NDEBUG
  fill_closedset[coord(0,0)] = true;
  auto searchResult = fill_closedset.begin(); 
#endif
  if (!map.ref(destination).accessible || searchResult == fill_closedset.end())
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
   // cout << "e";
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
  const double scalingFactor = 2; //// making this larger can make the pathfinding faster, at the cost of accuracy

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
    auto pos = paths.find(dest)->second.cameFrom;
    cout << "r";
    paths.erase(dest);
    auto it = reconstructPath(paths, pos);
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

vector<coord> Pather::getFillNeighbors(coord current)
  {
  /*vector<coord> output;
  if (!map.ref(current).accessible) // First, if the tile I am in isn't accessible, there are no neighbors.
    return output;
  // Check top, topleft, left, botleft, bot
  // topleft
  auto neighbor = map.ref(current.first - 1, current.second - 1);
  if (!neighbor.null && neighbor.accessible)
    output.push_back(neighbor.xy);

  neighbor = map.ref(current.first, current.second - 1); // top
  if (!neighbor.null && neighbor.accessible)
    output.push_back(neighbor.xy);

  neighbor = map.ref(current.first - 1, current.second); // left
  if (!neighbor.null && neighbor.accessible)
    output.push_back(neighbor.xy);

  neighbor = map.ref(current.first - 1, current.second +1); // bottom left
  if (!neighbor.null && neighbor.accessible)
    output.push_back(neighbor.xy);

  neighbor = map.ref(current.first, current.second + 1); // bottom
  if (!neighbor.null && neighbor.accessible)
    output.push_back(neighbor.xy);*/

  auto output = map.findNeighborList(current);


  vector<coord> out;
  for (auto it = output.begin(); it < output.end(); it++)
    {
    // check if it's in the closed set
    if (fill_closedset.find(it->xy) != fill_closedset.end())
      continue;
    out.push_back(it->xy);
    }
  return out;
  }

void Pather::stepFloodfill()
  {
  if (done)
    return;
  if ((fill_openset.size() == 0) || (fill_openset.begin() == fill_openset.end()))
    {
    done = true;
    ///// do the finishing stuff.
   /* if (metBot && metTop && metLeft)
      {
      for (int ycounter = 0; ycounter < map.height; ycounter++)
        for (int xcounter = 0; xcounter < map.width; xcounter++)
          {
          console->setCharForeground(xcounter, ycounter, TCODColor::orange);
          }
      }*/
    return;
    }

  coord currentpos = fill_openset.begin()->first;
  
  fill_openset.erase(currentpos);
  int val = 0;
  if (currentpos == start)
    val = 1;
  fill_closedset[currentpos] = val;
  auto neighbors = getFillNeighbors(currentpos);
  for (auto it = neighbors.begin(); it < neighbors.end(); it++)
    {
    if (!metBot)
      {
      if (it->second % map.height == map.height-1)
        metBot = true;
      }
    else if (!metTop)
      {
      if (it->second % map.height == 0)
        metTop = true;
      }
    if (!metLeft)
      {
      if (it->first % map.width == 0)
        metLeft = true;
      }
    fill_openset[*it] = 0;
    }
  
 // console->putCharEx(currentpos.first, currentpos.second, 249, TCODColor::lime, TCODColor::black);
  }

void Pather::completeFloodfill()
  {
start:
  cout << "p";
//  console->clear();
  metTop = metBot = metLeft = false;
  start.first = rand()%map.width;
  start.second = rand()%map.height;
  fill_openset.clear();
  fill_closedset.clear();
  fill_openset[start] = 0;
  while (!done)
    stepFloodfill();
  if (metTop == false || metBot == false || metLeft == false)
    goto start;
  
  //for (auto it = fill_closedset.begin(); it != fill_closedset.end(); it++)
  //  console->putCharEx(it->first.first % map.width, it->first.second % map.height, 249, TCODColor::peach, TCODColor::black);
  }