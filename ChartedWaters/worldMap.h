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
    
    static double getValue(noise::module::Perlin& gen, const int& x, const int& y, const double& zoom); // To have all code that accesses a Perlin noise gen for a world gen coordinate work the same way.
    virtual void gen();
    virtual maptile& ref(const int& x, const int& y);
    int getWidth();
    int getHeight();
    std::vector<coord> cities;
    long moistureSeed, altitudeSeed, ampSeed; // Amp seed = seed for perlin noise that determines amplitude
    long offsetSeed;

    std::vector<std::pair<coord, int>> boatpaths; // the paths and IDs of the boat settlers.

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

    std::vector<std::pair<coord, int>> path;
  private:
    void markZOC(WorldMapClass& wm, int cityCode);
    void recursiveZOC(const int& x,const int& y, WorldMapClass& wm, double size);
    void setRandomPos(const coord& dimensions); 
    
    std::mt19937 gen;
  };