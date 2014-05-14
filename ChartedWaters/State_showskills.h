#pragma once
#include "GameState.h"
#include "player.h"
#include "libtcod.hpp"

class State_ShowSkills : public GameState
{
public:
    State_ShowSkills(Player& captain);
    ~State_ShowSkills();
    virtual bool Init();

    virtual void Update(){}
    virtual void Render(TCODConsole *root);
    virtual void End();

    //
    virtual void Resize(int new_w, int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key, const int &unicode){}
    virtual void KeyDown(const int &key, const int &unicode);
    virtual void MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}

private:
    void redraw();
    TCODConsole* window;
    int w, h;
    Player& player;

};