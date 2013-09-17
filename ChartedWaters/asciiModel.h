/// A class that represents a rotatable set of ascii characters.

#pragma once
#include <libtcod.hpp>
#include <vector>
#include "vector3.h"


class ASCIIImage
  {
  public:
    ASCIIImage(const int& w, const int& h);
    ~ASCIIImage();
    double angle;
    void setAngle(const double& val); // redraws the map.
    TCODConsole* image;

    std::vector<int> original;

    int width;
    int height;
  private:
  };

class PieSlice
  {
  public:
    PieSlice(const double& r);
    PieSlice(const double& r, const double& angle);
    PieSlice(const double& minR, const double& maxR, const double& angle);
    ~PieSlice();
    vector3 direction;
    double arcAngle;
    void setAngle(const double& val); // redraws the map.
    void setDirection(const vector3& vec);
    void setDirection(const double& ang);
    virtual void redraw();
    TCODConsole* image;
    TCODColor color;
    static const double pi;
    double maxrange;
    double minrange;

  private:

  };

class SinglePieSlice : public PieSlice
  {
  public:
    SinglePieSlice(const double& pieAngle, const int& r);
    SinglePieSlice(const double& pieAngle, const int& r, const double& angle);

    double sliceAngle;
    virtual void redraw();
  private:

  };