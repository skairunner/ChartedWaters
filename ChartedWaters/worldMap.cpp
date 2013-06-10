#include "worldMap.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

WorldMapClass::WorldMapClass(const int& width, const int& height)
  :w(width), h(height)
  {
  grid.reserve(sizeof(maptile) * w * h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      grid.push_back(maptile());
  }

void WorldMapClass::gen()
  {
  srand(time(0));
  for (int counter = 0; counter < 50; counter++)
    rand();
  noise::module::Perlin moisture;
  noise::module::Perlin altitude;
  moisture.SetSeed(rand());
  altitude.SetSeed(rand());
  vector<float> buffer(w * h, 0);
  double zoom = 0.07;
  // altitude!
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      double temp = altitude.GetValue((xcounter+1) * zoom + 0.01, (ycounter+1) * zoom + 0.01, 2.5);
      temp -= 0.25;
      buffer[xcounter + ycounter * w] = temp > 0 ? temp * 20 : 0; // we don't need buffer right now
      ref(xcounter, ycounter).altitude = buffer[xcounter + ycounter * w];
      }

    // moisture!
    double zoom2 = 0.05;
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      
      double temp = moisture.GetValue((xcounter+1) * zoom2 + 0.01, (ycounter+1) * zoom2 + 0.01, 0.5);
      if (ycounter > h * 0.3)
        temp += 0.4;
      else if (ycounter > h * 0.6 && ycounter < h * 0.8)
        temp += 0.7;
      temp += 0.0;
      // buffer[xcounter + ycounter * w] = temp > 0 ? temp * 20 : 0;
      ref(xcounter, ycounter).moisture = temp;
      }
  }

maptile& WorldMapClass::ref(const int& x, const int& y)
  {
  if (x >= 0 && y >= 0 && x < w && y < h)
    return grid[x + y * w];
  else
    return null;
  }

int WorldMapClass::getWidth()
  {
  return w;
  }

int WorldMapClass::getHeight()
  {
  return h;
  }