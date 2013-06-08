#pragma once

#include "GameState.h"

class MainGameState: public GameState
  {
  public:
  GameState *prev;
  MainGameState(const int& w, const int& h, std::vector<GameState*>* pointerToStack, const int debugCounter);
  ~MainGameState();
  virtual bool Init();
  virtual void Update();
  virtual void Render(TCODConsole *root);
  virtual void End();
  //
  virtual void Resize(int new_w,int new_h);
  virtual void WindowActive();
  virtual void WindowInactive();
  virtual void KeyUp(const int &key,const int &unicode);
  virtual void KeyDown(const int &key,const int &unicode);
  virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
  virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
  virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);

  protected:
    TCODConsole* console;
    int debugcounter;
    int width, height;
  };
