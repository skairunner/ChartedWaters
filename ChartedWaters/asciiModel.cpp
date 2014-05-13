#include "asciiModel.h"
#include <cmath>

const double PieSlice::pi = 3.1415926535897932384626433832795f;

ASCIIImage::ASCIIImage(const int& w, const int& h)
  : angle(0), width(w), height(h), original(w * h, 0)
  {
  image = new TCODConsole(w, h);
  image->setKeyColor(TCODColor(255,0,255));
  image->setDefaultBackground(TCODColor(255, 0, 255));
  }

ASCIIImage::~ASCIIImage()
  {
  delete image;
  }

void ASCIIImage::setAngle(const double& val)
  {
  
  }

PieSlice::PieSlice(const double& r)
  : arcAngle(pi/2), maxrange(r), minrange(0), direction(r, r, 0, r, 0, 0)
  {
  image = new TCODConsole((int)(2 * r) + 1, (int)(2 * r) + 1);
  image->setKeyColor(TCODColor(255,0,255));
  image->setDefaultBackground(TCODColor(255, 0, 255));
  color = TCODColor::lighterBlue;
  setAngle(arcAngle);
  }

PieSlice::PieSlice(const double& r, const double& angle)
  : arcAngle(angle), maxrange(r), minrange(0), direction(r, r, 0, r, 0, 0)
  {
  image = new TCODConsole((int)(2 * r) + 1, (int)(2 * r) + 1);
  image->setKeyColor(TCODColor(255,0,255));
  image->setDefaultBackground(TCODColor(255, 0, 255));
  color = TCODColor::lighterBlue;
  setAngle(arcAngle);
  }

PieSlice::PieSlice(const double& minR, const double& maxR, const double& angle)
  : arcAngle(angle), maxrange(maxR), minrange(minR), direction(maxR, maxR, 0, maxR, 0, 0)
  {
  image = new TCODConsole((int)(2 * maxR) + 1 , (int)(2 * maxR) + 1);
  image->setKeyColor(TCODColor(255,0,255));
  image->setDefaultBackground(TCODColor(255, 0, 255));
  color = TCODColor::lighterBlue;
  setAngle(arcAngle);
  }

PieSlice::~PieSlice()
  {
  delete image;
  }

void PieSlice::setAngle(const double& val)
  {
  arcAngle = val;
  redraw();
  }

void PieSlice::setDirection(const vector3& vec)
  {
  direction = vec;
  redraw();
  }

void PieSlice::setDirection(const double& ang)
  {
  setDirection(vector3(cos(ang), sin(ang), 0));
  }

void PieSlice::redraw()
  {
  vector3 perpen1(-direction.y, direction.x, 0);
  vector3 perpen2 = -1 * perpen1;
  long double maxDistSqr = maxrange * maxrange;
  long double minDistSqr = minrange * minrange;
  image->clear();

  for (int ycounter = 0; ycounter < image->getHeight(); ycounter++) 
    for (int xcounter = 0; xcounter < image->getWidth(); xcounter++)
      {
      long double xdist = maxrange - xcounter; xdist *= xdist;
      long double ydist = maxrange - ycounter; ydist *= ydist;
      if (xdist + ydist < maxDistSqr && xdist + ydist > minDistSqr) // the tile is far/close enough from the origin
        {
        vector3 position(maxrange, maxrange, 0, xcounter, ycounter, 0);
        if (acos(perpen1.dot(position)/(perpen1.length * position.length)) < arcAngle/2.0f ||  acos(perpen2.dot(position)/(perpen2.length * position.length)) < (arcAngle/2.0f))
          // if the angle between (origin to the dot) and the vector perpendicular to the direction is within range...
          {
          image->setCharBackground(xcounter, ycounter, color);
          continue;
          }
        image->setCharBackground(xcounter, ycounter, TCODColor(255,0,255));
        }
      }
  }

/////
/////
/////

SinglePieSlice::SinglePieSlice(const double& pieAngle, const int& r)
  : PieSlice(r), sliceAngle(pieAngle)
  {

  }

SinglePieSlice::SinglePieSlice(const double& pieAngle, const int& r, const double& angle)
  : PieSlice(r, angle), sliceAngle(pieAngle)
  {

  }

void SinglePieSlice::redraw()
  {
  long double maxDistSqr = maxrange * maxrange;
  long double minDistSqr = minrange * minrange;
  vector3 sliceVector(direction.x * cos(sliceAngle) - direction.y * sin(sliceAngle), direction.x * sin(sliceAngle) - direction.y * cos(sliceAngle), 0);
  image->clear();

  for (int ycounter = 0; ycounter < image->getHeight(); ycounter++) 
    for (int xcounter = 0; xcounter < image->getWidth(); xcounter++)
      {
      long double xdist = maxrange - xcounter; xdist *= xdist;
      long double ydist = maxrange - ycounter; ydist *= ydist;
      if (xdist + ydist < maxDistSqr && xdist + ydist > minDistSqr) // the tile is far/close enough from the origin
        {
        vector3 position(maxrange, maxrange, 0, xcounter, ycounter, 0);
        if (acos(sliceVector.dot(position)/(sliceVector.length * position.length)) < arcAngle/2.0f)
          {
          image->setCharBackground(xcounter, ycounter, color);
          continue;
          }
        image->setCharBackground(xcounter, ycounter, TCODColor(255,0,255));
        }
      }
  }