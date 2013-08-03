#pragma once
#include <vector>
#include "include/noise.h"
#include <random>

typedef std::pair<int, int> coord;

struct maptile
  {
  maptile(): owner(0), moisture(0), altitude(0), isCity(false), isCoastal(false), isNull(false), isInZOC(0){}
  float moisture;
  float altitude;
  int owner;
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
    virtual void gen();
    maptile& ref(const int& x, const int& y);
    int getWidth();
    int getHeight();
    std::vector<coord> cities;
    long moistureSeed, altitudeSeed;

  protected:
    void setCoastFlags();
    void setCityFlags();
    bool setCoastal(const int& xcounter, const int& ycounter);
    
    int w, h;
    maptile null;
    std::vector<maptile> grid;
    void setFactionsCity(const int& faction, const int& numberOfCities, const unsigned long int& seed);
  };

class randomBoat // randomly walks.
  {
  public:
    randomBoat(const coord& dim, const int& ffaction = 1, unsigned long int seed = 1243523642, const coord& start = coord(-1, -1));
    void setSeed(unsigned long int seed);
    int faction;
    coord startingPosition;
    coord currentPosition;
    bool explore(WorldMapClass& wm);
    bool tryMove(WorldMapClass& wm); // Return false if you can't move.
    void setRandomPosUntilSea(WorldMapClass& wm);
  private:
    void markZOC(WorldMapClass& wm, int cityCode);
    void recursiveZOC(const int& x,const int& y, WorldMapClass& wm, double size);
    void setRandomPos(const coord& dimensions); 
    
    std::mt19937 gen;
  };