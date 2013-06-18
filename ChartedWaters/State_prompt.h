#pragma once
#include <SDL.h>
#include "GameState.h"
#include <string>

class State_Prompt : public GameState // Yes / no.
  {
  public:
    State_Prompt(const int& w, const int& h, const std::string& message, bool& result);
    bool Init();
    void Render(TCODConsole *root);
    virtual void KeyDown(const int &key,const int &unicode);
    void End();
  private:
    void redraw();
    TCODConsole* console;
    bool& refToResult;
    const std::string msg;
    int width, height;
  };