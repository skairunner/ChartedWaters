#include "State_Combat.h"
#include "vector3.h"
#include <SDL.h>
#include "include/noise.h"
#include <vector>
#include "World.h" //test
#include <string>
#include <iostream>
#include "State_ShipStatus.h"

using namespace std;

CombatMap::CombatMap(const long& altSeed, const long& moistSeed, const int& xc, const int& yc)
: WorldMapClass(200, 200), x(xc), y(yc)
{
    moistureSeed = moistSeed;
    altitudeSeed = altSeed;
    gen();
}

void CombatMap::gen()
{
    for (int counter = 0; counter < 50; counter++)
        rand();
    cities.clear();
    noise::module::Perlin moisture;
    noise::module::Perlin altitude;
    moisture.SetSeed(moistureSeed);
    altitude.SetSeed(altitudeSeed);

    vector<float> buffer(w * h, 0);
    const double ozoom = 0.07;
    const double zoom = 0.0007;
    // altitude!
    for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
    {
        double temp = altitude.GetValue(x * ozoom + (xcounter + 1) * zoom + 0.01, y * ozoom + (ycounter + 1) * zoom + 0.01, 2.5);
        temp -= 0.25;
        if (temp < 0)
        {
            if (-temp > 0.3)
                temp *= 20;
            else
                temp *= 10;
            buffer[xcounter + ycounter*w] = (float)temp;
        }
        else
            buffer[xcounter + ycounter * w] = (float)(temp * 20);// > 0 ? temp * 20 : 0; // we don't need buffer right now
        ref(xcounter, ycounter).altitude = buffer[xcounter + ycounter * w];
        ref(xcounter, ycounter).isCoastal = false;
        ref(xcounter, ycounter).isCity = false;
        ref(xcounter, ycounter).isInZOC = false;
    }

    // moisture!
    const double zoom2 = 0.0005;
    for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
    {

        double temp = moisture.GetValue(x * ozoom + (xcounter + 1) * zoom2 + 0.01, y * ozoom + (ycounter + 1) * zoom2 + 0.01, 0.5);
        if (ycounter > h * 0.32)
            temp += 0.4;
        else if (ycounter > h * 0.6 && ycounter < h * 0.8)
            temp += 0.7;
        temp += 0.0;
        // buffer[xcounter + ycounter * w] = temp > 0 ? temp * 20 : 0;
        ref(xcounter, ycounter).moisture = (float)temp;
    }

    setCoastFlags();
}

maptile& CombatMap::ref(const int& x, const int& y)
{
    if (y >= 0 && y < h && x >= 0 && x < w)
        return grid[x + y * w];
    else
        return null;
}

maptile& CombatMap::ref(const coord& pos)
{
    return ref(pos.first, pos.second);
}

maptile& CombatMap::ref(const coord_d& pos)
{
    return ref((int)pos.first, (int)pos.second);
}

//////
//////
//////

CombatShip::CombatShip(Ship& rToShip)
: refToShip(rToShip), angle(0), localPosition(50, 50), speed(0), sailState(SS_CLOSED), targetAngle(0)
, testrange(5, 10, PieSlice::pi / 2)
{
    testrange.color = TCODColor::green;
}

void CombatShip::step()
{
    speed -= 1;
    switch (sailState)
    {
    case SS_ANCHOR:
        speed -= 0.1;
        break;
    case SS_CLOSED:
        speed += 0.3;
        break;
    case SS_QUARTER:
        speed += 0.7;
        break;
    case SS_HALF:
        speed += 1.1;
        break;
    case SS_THREEQUARTERS:
        speed += 1.5;
        break;
    case SS_FULL:
        speed += 1.9;
        break;
    default:
        break;
    }

    if (speed > 0)
    {
        if (targetAngle > angle)
        {
            angle += refToShip.getTurning() * 0.04;
            if (angle > targetAngle)
                angle = targetAngle;
            speed *= 0.9;
            testrange.setDirection(-angle);
        }
        if (angle > targetAngle)
        {
            angle -= refToShip.getTurning() * 0.04;
            if (angle < targetAngle)
                angle = targetAngle;
            speed *= 0.9;
            testrange.setDirection(-angle);
        }
    }

    if (speed > refToShip.getSpeed())
        speed = refToShip.getSpeed();
    if (speed < 0)
        speed = 0;

    for (int counter = 0; counter < 10; counter++)
    if (trail.size() > 10)
        trail.pop_front();

    double newPosX = localPosition.first + cos(angle) * speed / 2;
    double newPosY = localPosition.second - sin(angle) * speed / 2;
    for (int counter = 0; counter < 10; counter++)
    {
        localPosition.first += cos(angle) * speed / 20;
        localPosition.second += -sin(angle) * speed / 20;
        trail.push_back(localPosition);
    }

    localPosition.first = newPosX;
    localPosition.second = newPosY;
}

void CombatShip::normalizeAngles()
{
    // simplify
    if (angle > 2 * PieSlice::pi) angle -= 2 * PieSlice::pi;
    if (angle < -2 * PieSlice::pi) angle += 2 * PieSlice::pi;

    // now make it into +- angles
    if (angle > PieSlice::pi)
        angle = 2 * PieSlice::pi - angle;
    else if (angle < -PieSlice::pi)
        angle = 2 * PieSlice::pi + angle;
}

int CombatShip::getDirectionToTurn(const double& target)
{
    if (angle > 0 && target < 0)
    {
        if (abs(angle - (PieSlice::pi * 2 + target)) > abs(target - angle))
            return SS_CLOCKWISE;
        else return SS_COUNTERCLOCKWISE;
    }
    if (angle < 0 && target > 0)
    {
        if (abs(target - angle) > abs(angle + PieSlice::pi * 2 - target))
            return SS_CLOCKWISE;
        else return SS_COUNTERCLOCKWISE;
    }
    return -1;
}

double CombatShip::getAngleDifference(const double& target)
{
    if (angle > 0 && target < 0)
    {
        double a1 = abs(angle - (PieSlice::pi * 2 + target));
        double a2 = abs(target - angle);
        return a1 > a2 ? a2 : a1;
    }
    if (angle < 0 && target > 0)
    {
        double a1 = abs(angle - (angle + PieSlice::pi * 2 - target));
        double a2 = abs(target - angle);
        return a1 > a2 ? a2 : a1;
    }
    return -1;
}

void CombatShip::setTargetAngle(double target)
{
    normalizeAngles();
    if (target * angle >= 0)
        targetAngle = target;
    else
    {
        int direction = getDirectionToTurn(target);
        double diff = getAngleDifference(target);
        if (direction == SS_COUNTERCLOCKWISE)
            targetAngle = angle + diff;
        else if (direction == SS_CLOCKWISE)
            targetAngle = angle - diff;
        else cout << "Error in CombatShip::setTargetAngle()\n";
    }
}

double CombatShip::getTargetAngle()
{
    return targetAngle;
}


//////
//////
//////

/*
State_Combat::State_Combat(const int& wwidth, const int& hheight)
: left(PieSlice::pi/2, 16, PieSlice::pi * 0.5f ), right(PieSlice::pi * 1.5, 16, PieSlice::pi * 0.5f), forward(0, 8, PieSlice::pi / 3), width(wwidth), height(hheight),
backward(PieSlice::pi, 20, PieSlice::pi / 4)
{
console = new TCODConsole(width, height);
x = 100 / 2 - 20;
y = 48 / 2 - 20;
angle = PieSlice::pi / 4;
angle2 = angle;
left.minrange = 5;
right.minrange = 5;
forward.minrange = 5;
backward.minrange = 17;

forward.color = TCODColor::lighterYellow;
backward.color = TCODColor::white;
left.color = TCODColor::lighterGreen;
right.color = TCODColor::lighterRed;

left.redraw();
right.redraw();
forward.redraw();
backward.redraw();
}*/

State_Combat::State_Combat(Fleet& fleet, const long& altSeed, const long& moistSeed, const int& xcoord, const int& ycoord)
: map(altSeed, moistSeed, xcoord, ycoord), scrollspeed(5), playerFleet(fleet), mouseX(0), mouseY(0), mouseRightClick(false), redraw(false),
turn(0)
{
    focusX = 100;
    focusY = 50;
    console = new TCODConsole(200, 200);
    shipconsole = new TCODConsole(200, 200);
    rangeconsole = new TCODConsole(200, 200);
    trailconsole = new TCODConsole(200, 200);

    int startx = 100;
    int starty = 100;
    int dX = -2;
    int dY = 0;

    for (auto it = fleet.ships.begin(); it != fleet.ships.end(); it++)
    {
        playerShips.push_back(CombatShip(it->second));
        playerShips.back().localPosition = coord(startx, starty);
        startx -= dX;
        starty -= dY;
    }

    pages = getKeys(fleet.ships);
    pageit = 0;
    page = pages[pageit];
}

bool State_Combat::Init()
{
    Renderer::getHighResTerrainBitmap(console, map);
    updateShips();

    return true;
}

void State_Combat::End()
{
    delete console;
    delete shipconsole;
    delete rangeconsole;
    delete trailconsole;
}

void State_Combat::Update()
{
    if (mouseRightClick)
    {
        vector3 newDirection(mouseX, mouseY, 0, playerShips[pageit].localPosition.first, playerShips[pageit].localPosition.second, 0);
        vector3 xAxis(1, 0, 0);
        double result = newDirection.dot(xAxis) / newDirection.length;
        result = acos(result);
        if (mouseY > playerShips[pageit].localPosition.second)
            result *= -1;
        playerShips[pageit].setTargetAngle(result);
        mouseRightClick = false;
    }

    if (update)
    {
        update = false;
        updateShips();
    }
    if (redraw)
    {
        redraw = false;
        highlighted.clear();
        redrawShips();
    }

    if (lock)
        lockToShip();
    /*  angle += 0.05;
      angle2 -= 0.05;
      left.setDirection(vector3(cos(angle), sin(angle), 0));
      right.setDirection(vector3(cos(angle), sin(angle), 0));
      forward.setDirection(vector3(cos(-angle), sin(-angle), 0));
      backward.setDirection(vector3(cos(-angle), sin(-angle), 0));*/
}

void State_Combat::Render(TCODConsole *root)
{
    root->setKeyColor(TCODColor(255, 0, 255));

   /* TCODConsole::blit(console, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 1.0f, 1.0f);
    //testPair.setDirection(-player.angle);
    TCODConsole::blit(player.testrange.image, 0, 0, 0, 0, rangeconsole, (int)(player.localPosition.first - player.testrange.maxrange), (int)(player.localPosition.second - player.testrange.maxrange), 1.0f, 1.0f);
   */ 
    TCODConsole::blit(console, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 1.0f, 1.0f);        
    TCODConsole::blit(trailconsole, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
    TCODConsole::blit(shipconsole, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
    TCODConsole::blit(rangeconsole, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.5f);

    for (coord xy : highlighted)
        root->setCharBackground(xy.first - focusX - screenwidth/2, xy.second - focusY - screenheight/2, TCODColor::white);

    string state;
    switch (playerShips[pageit].sailState)
    {
    case SS_ANCHOR:
        state = "Anchored";
        break;
    case SS_CLOSED:
        state = "Sails closed";
        break;
    case SS_QUARTER:
        state = "Quarter sail";
        break;
    case SS_HALF:
        state = "Half sail";
        break;
    case SS_THREEQUARTERS:
        state = "Three-quarters sail";
        break;
    case SS_FULL:
        state = "Full sail";
        break;
    default:
        state = "Error state";
        break;
    }

    
    int line = 0;
    root->setDefaultForeground(TCODColor::yellow);
    root->print(0, line++, ("Turn " + to_string(turn/10.0f)).c_str());
    root->setDefaultForeground(TCODColor::white);
    root->print(0, line++, ("Ship " + to_string(pageit+1)).c_str());
    root->print(0, line++, state.c_str());
    root->print(0, line++, "Speed: %.2f", playerShips[pageit].speed);
    root->print(0, line++, "Angle: %.2f degrees", playerShips[pageit].angle * 180 / PieSlice::pi);
    root->print(0, line++, "Target angle: %.2f degrees", playerShips[pageit].getTargetAngle() * 180 / PieSlice::pi);
    /* root->setKeyColor(TCODColor(255,0,255));
     root->print(0, 0, "Angle: %.2f", angle * 180.0f / PieSlice::pi);
     TCODConsole::blit(left.image, 0, 0, 0, 0, root, x, y, 0.0f, 0.5f);
     TCODConsole::blit(right.image, 0, 0, 0, 0, root, x, y, 0.0f, 0.5f);
     TCODConsole::blit(forward.image, 0, 0, 0, 0, root, x + (left.maxrange - forward.maxrange), y  + (left.maxrange - forward.maxrange), 0.0f, 0.5f);
     TCODConsole::blit(backward.image, 0, 0, 0, 0, root, x + (left.maxrange - backward.maxrange), y  + (left.maxrange - backward.maxrange), 0.0f, 0.5f);
     root->putCharEx(x + left.maxrange, y + left.maxrange, 9, TCODColor::red, TCODColor::black);*/
}

void State_Combat::KeyDown(const int &key, const int &unicode)
{
    /* if (key == SDLK_UP)
       angle += 0.05;
       else if (key == SDLK_DOWN)
       angle -= 0.05;
       else */

    switch (key)
    {
    case SDLK_ESCAPE:
        popMe = true;
        break;
    case SDLK_RIGHT:
        focusX = (focusX + scrollspeed <= 200 - screenwidth / 2) ? focusX + scrollspeed : 200 - screenwidth / 2;
        redraw = true;
        break;
    case SDLK_LEFT:
        focusX = (focusX - scrollspeed >= screenwidth / 2) ? focusX - scrollspeed : screenwidth / 2;
        redraw = true;
        break;
    case SDLK_UP:
        focusY = (focusY - scrollspeed >= screenheight / 2) ? focusY - scrollspeed : screenheight / 2;
        redraw = true;
        break;
    case SDLK_DOWN:
        focusY = (focusY + scrollspeed <= 200 - screenheight / 2) ? focusY + scrollspeed : 200 - screenheight / 2;
        redraw = true;
        break;
    case SDLK_s:
        nextState = new State_ShipStatus(playerFleet);
        pushSomething = true;
        break;
    case SDLK_y:
        lock = !lock;
        break;
    default:
        break;
    }

    switch (unicode)
    {
    case '+':
        if (playerShips[pageit].sailState < SS_FULL)
            playerShips[pageit].sailState++;
        break;
    case '-':
        if (playerShips[pageit].sailState > SS_ANCHOR)
            playerShips[pageit].sailState--;
        break;
    case '.':
        update = true;
        turn++;
        break;
    case '>':
        playerShips[pageit].setTargetAngle(playerShips[pageit].getTargetAngle() + 0.05);
        break;
    case '<':
        playerShips[pageit].setTargetAngle(playerShips[pageit].getTargetAngle() - 0.05);
        break;
    case '[':
        if (pageit > 0)
        {
            pageit--;
            page = pages[pageit];
            redraw = true;
        }
        break;
    case ']':
        if (pageit < pages.size() - 1)
        {
            pageit++;
            page = pages[pageit];
            redraw = true;
        }
        break;
    default:
        break;
    }
}

void State_Combat::lockToShip() // sets camera to ship.
{
    auto pos = playerShips[pageit].localPosition;
    focusX = pos.first < screenwidth / 2 ? screenwidth / 2 : (int)pos.first; // Make sure it's within bounds.
    focusY = pos.second < screenheight / 2 ? screenheight / 2 : (int)pos.second;
    focusX = focusX > 200 - screenwidth / 2 ? 200 - screenwidth / 2 : focusX;
    focusY = focusY > 200 - screenheight / 2 ? 200 - screenheight / 2 : focusY;
}

void State_Combat::MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{
    mouseX = focusX + (iX / 12 - screenwidth / 2);
    mouseY = focusY + (iY / 12 - screenheight / 2);
}

void State_Combat::MouseButtonDown(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{
    /*if (iButton == SDL_BUTTON_LEFT)
      mouseClick = true;*/
    if (iButton == SDL_BUTTON_RIGHT)
        mouseRightClick = true;
}

coord State_Combat::displace(const coord& pos)
{
    for (int counter = 1;; counter++)
    {

        int length = counter * 2 + 1;
        // top edge, bottom edge
        for (int edge = 0; edge < length; edge++)
        {
            maptile& refToTile = map.ref(pos.first - counter + edge, pos.second - counter);
            if (refToTile.altitude <= 0 && !refToTile.isNull && !refToTile.isCoastal)
                return coord(pos.first - counter + edge, pos.second - counter);
        }
        for (int edge = 0; edge < length; edge++)
        {
            maptile& refToTile = map.ref(pos.first - counter + edge, pos.second + counter);
            if (refToTile.altitude <= 0 && !refToTile.isNull && !refToTile.isCoastal)
                return coord(pos.first - counter + edge, pos.second + counter);
        }

        // left, right edges
        for (int edge = 0; edge < length; edge++)
        {
            maptile& refToTile = map.ref(pos.first - counter, pos.second - counter + edge);
            if (refToTile.altitude <= 0 && !refToTile.isNull && !refToTile.isCoastal)
                return coord(pos.first - counter, pos.second - counter + edge);
        }
        for (int edge = 0; edge < length; edge++)
        {
            maptile& refToTile = map.ref(pos.first + counter, pos.second - counter + edge);
            if (refToTile.altitude <= 0 && !refToTile.isNull && !refToTile.isCoastal)
                return coord(pos.first + counter, pos.second - counter + edge);
        }

    }
    return coord(-1, -1);
}

void State_Combat::updateShips()
{
    shipconsole->clear();

    rangeconsole->setDefaultBackground(TCODColor(255, 0, 255));
    rangeconsole->setKeyColor(TCODColor(255, 0, 255));
    rangeconsole->clear();

    trailconsole->clear();

    for (auto it = playerShips.begin(); it < playerShips.end(); it++)
    {
        it->step();
    }

    int counter = 0;
    for (auto it = playerShips.begin(); it < playerShips.end(); it++)
    {
        int x = it->localPosition.first;
        int y = it->localPosition.second;
        if (x < 0 || x > 200 || y < 0 || y > 200)
            popMe = true;

        blitShip(*it);
        counter++;
    }
}

void State_Combat::redrawShips()
{
    int counter = 0;
    for (auto it = playerShips.begin(); it < playerShips.end(); it++)
    {
        if (counter == pageit)
            highlighted.push_back(it->localPosition);
        blitShip(*it);
        counter++;
    }
}

void State_Combat::blitShip(CombatShip& ship)
{
    int x = (int)ship.localPosition.first;
    int y = (int)ship.localPosition.second;
    int c = ship.refToShip.character;

    //Check that coords are not on land
    if (map.ref(x, y).altitude > 0)
    {
        auto coords = displace(coord(x, y));
        if (coords.first == -1 && coords.second == -1)
        {
            cout << "Error: did not find non-land tile\n";
            popMe = true;
            return;
        }
        ship.localPosition = coords;
        x = coords.first;
        y = coords.second;
        // Since the ship presumably crashed into land, take some HP off, scaling w/square of speed.
        ship.refToShip.durability -= int(ship.speed * ship.speed + 1) * 10;
        ship.speed = 0;
        ship.trail.clear();
    }
    shipconsole->putCharEx(x, y, c, Renderer::findFactionColor(ship.refToShip.captain.faction), TCODColor::black);
    // Draw the trails next
    
    float intensity = 1;
    for (auto it = ship.trail.rbegin(); it < ship.trail.rend(); it++)
    {
        trailconsole->putCharEx((int)it->first, (int)it->second, 177, TCODColor::lerp(TCODColor::darkBlue, TCODColor::white, intensity), TCODColor::black);
        intensity -= 0.05;
    }
}