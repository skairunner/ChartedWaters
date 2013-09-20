#pragma once
#include <vector>
#include "utility.h"


// A generic grid to be used to handle collisions and interaction.
class EntityMap
  {
  public:
    EntityMap(const int& w, const int& h);
    int timestamp;
    void setEntity(const int& ID, const int& x, const int& y);
    void setEntity(const int& ID, const coord& coords);
    std::vector<int> getEntityList(const int& x, const int& y);
    std::vector<int> getEntityList(const coord& coords);
    void clear();
    void step();

  private:
    std::vector<std::pair<int, std::vector<int>>> grid; // timestamp / ships
    // if the timestamp is old, ignore. If a new stamp is written on an old stamp, clear() the pair.second
    std::pair<int, std::vector<int>>& ref(const int& x, const int& y);
    int width;
    int height;
    std::pair<int, std::vector<int>> null;
  };