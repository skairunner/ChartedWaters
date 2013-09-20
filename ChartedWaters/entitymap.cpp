#include "entitymap.h"

using namespace std;

EntityMap::EntityMap(const int& w, const int& h)
  : timestamp(0), grid(w*h, std::pair<int, std::vector<int>>(0, std::vector<int>())), width(w), height(h)
  {
  null.first = -1;
  }

void EntityMap::step()
  {
  timestamp++;
  if (timestamp < 0)
    {
    clear();
    timestamp = 0;
    }
  }

void EntityMap::setEntity(const int& ID, const int& x, const int& y)
  {
  std::pair<int, std::vector<int>>& reference = ref(x, y);
  if (timestamp == -1)
    return;
  if (timestamp == ref(x, y).first)
    {
    reference.second.push_back(ID);
    }
  else
    {
    reference.second.clear();
    reference.second.push_back(ID);
    reference.first = timestamp;
    }
  }

void EntityMap::setEntity(const int& ID, const coord& coords)
  {
  setEntity(ID, coords.first, coords.second);
  }

std::vector<int> EntityMap::getEntityList(const int& x, const int& y)
  {
  auto reference = ref(x, y);
  if (reference.first == timestamp)
    return reference.second;

  return vector<int>();
  }

std::vector<int> EntityMap::getEntityList(const coord& coords)
  {
  return getEntityList(coords.first, coords.second);
  }

void EntityMap::clear()
  {
  for (int ycounter = 0; ycounter < height; ycounter++)
    for (int xcounter = 0; xcounter < width; xcounter++)
      {
      grid[xcounter + ycounter * width].first = 0;
      grid[xcounter + ycounter * width].second.clear();
      }
  }

std::pair<int, std::vector<int>>& EntityMap::ref(const int& x, const int& y)
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