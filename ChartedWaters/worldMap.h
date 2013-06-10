#pragma once
#include <vector>
#include "include/noise.h"

struct maptile
  {
  maptile(): moisture(0), altitude(0){}
  float moisture;
  float altitude;
  };

class WorldMapClass
  {
  public:
    WorldMapClass(const int& width, const int& height);
    void gen();
    maptile& ref(const int& x, const int& y);
    int getWidth();
    int getHeight();

  private:
    int w, h;
    maptile null;
    std::vector<maptile> grid;
  };
