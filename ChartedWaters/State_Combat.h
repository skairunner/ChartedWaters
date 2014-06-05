#pragma once
#include "GameState.h"
#include "asciiModel.h"
#include "worldMap.h"
#include <libtcod.hpp>
#include <deque>
#include "Ship.h"

typedef std::pair<double, double> coord_d;

enum SailState{ SS_ANCHOR = 1, SS_CLOSED, SS_QUARTER, SS_HALF, SS_THREEQUARTERS, SS_FULL };
enum Rotation{ SS_COUNTERCLOCKWISE = 1, SS_CLOCKWISE };

class CombatShip
{
public:

    CombatShip(Ship& rToShip);
    Ship& refToShip;
    coord_d localPosition;
    double angle;
    void setTargetAngle(double target);
    double getTargetAngle();
    double speed;
    int sailState;
    void step();
    std::deque<coord_d> trail;

    PieSlice testrange;
    int owner;
private:
    double targetAngle;
    void normalizeAngles(); // if angle > 2pi + a, angle = a. 
    // if angle < -2pi + a (a < 0), angle = a

    int getDirectionToTurn(const double& target);
    double getAngleDifference(const double& target);
};

class CombatMap : public WorldMapClass
{
public:
    CombatMap(const long& altSeed, const long& moistSeed, const int& xc, const int& yc);
    virtual void gen();
    maptile& ref(const coord& pos);
    virtual maptile& ref(const int& x, const int& y);
    maptile& ref(const coord_d& pos);
private:
    int x, y; // coords
};

class State_Combat : public GameState
{
public:
    // State_Combat(const int& wwidth, const int& hheight);
    State_Combat(Fleet& fleet, const long& altSeed, const long& moistSeed, const int& xcoord, const int& ycoord);
    virtual bool Init();

    virtual void Update();
    virtual void Render(TCODConsole *root);
    virtual void End();
    virtual void RecoverFromPush() { if (!nextState) delete nextState; pushSomething = false; }
    //
    virtual void Resize(int new_w, int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key, const int &unicode){}
    virtual void KeyDown(const int &key, const int &unicode);
    virtual void MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY);
    virtual void MouseButtonUp(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY);

private:

    CombatMap map;

    std::vector<CombatShip> playerShips;
    Fleet& playerFleet;
    int page, pageit;
    std::vector<int> pages;

    void lockToShip();
    bool mouseClick, mouseRightClick;
    coord displace(const coord& pos);

    void updateShips();
    void redrawShips();
    void blitShip(CombatShip& ship);

    std::vector<CombatShip> shipList;

    const int scrollspeed;
    int turn; // divide by 10 for actual turns.
    bool update, redraw;
    bool lock;
    TCODConsole* console;
    TCODConsole* trailconsole;
    TCODConsole* shipconsole;
    TCODConsole* rangeconsole;
    std::vector<coord> highlighted;
    int x, y;
    int mouseX, mouseY;
    int focusX, focusY;
};