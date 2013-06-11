#pragma once
#include <vector>
#include "include/noise.h"
#include <random>
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>

//using namespace boost::random;
typedef std::pair<int, int> coord;

struct maptile
  {
  maptile(): moisture(0), altitude(0), isCity(false), isCoastal(false), isNull(false), isInZOC(0) {}
  float moisture;
  float altitude;
  int isInZOC;
  int isCity;
  bool isCoastal;
  bool isNull;
  };

class randomBoat;

class WorldMapClass
  {
  public:
    friend class randomBoat;
    WorldMapClass(const int& width, const int& height);
    void gen();
    maptile& ref(const int& x, const int& y);
    int getWidth();
    int getHeight();
    std::vector<coord> cities;

  private:
    void setCoastFlags();
    void setCityFlags();
    int w, h;
    maptile null;
    std::vector<maptile> grid;
    void setFactionsCity(const int& faction, const int& numberOfCities);
  };

class randomBoat // randomly walks.
  {
  public:
    randomBoat(const coord& dim, const int& ffaction = 1, const coord& start = coord(-1, -1));
    int faction;
    coord startingPosition;
    coord currentPosition;
    bool explore(WorldMapClass& wm);
    bool tryMove(WorldMapClass& wm); // Return false if you can't move.
    void setRandomPosUntilSea(WorldMapClass& wm);
  private:
    void markZOC(WorldMapClass& wm);
    void recursiveZOC(const int& x,const int& y, WorldMapClass& wm, double budget);
    void setRandomPos(const coord& dimensions); 
    
    std::mt19937 gen;
  };