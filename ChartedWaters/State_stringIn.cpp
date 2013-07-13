#include "State_stringIn.h"
#include <SDL.h>

using namespace std;

state_StringIn::state_StringIn(int w, string& input, std::string& displaytext)
  :inputRef(input), width (w), text(displaytext)
  {
  console = new TCODConsole(width+2, 4);
  }

state_StringIn::~state_StringIn()
  {
  delete console;
  }

bool state_StringIn::Init()
  {

  return true;
  }
void state_StringIn::Update()
  {
  
  }

void state_StringIn::Render(TCODConsole *root)
  {
  console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->print(1, 1, text.c_str());
  console->print(1, 2, (string("> ") + buffer).c_str());
  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0, 1, 1);
  }
void state_StringIn::End()
  {
  inputRef = buffer;
  }
  //
void state_StringIn::Resize(int new_w,int new_h){}
void state_StringIn::WindowActive(){}
void state_StringIn::WindowInactive(){}
void state_StringIn::KeyUp(const int &key,const int &unicode){}

void state_StringIn::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_BACKSPACE)
    {
    if (buffer.size() != 0)
      buffer.pop_back();
    }
  else if (key == SDLK_RETURN)
    popMe = true;
  else if (key == SDLK_ESCAPE)
    {
    buffer.clear();
    popMe = true;
    }
  else if (unicode)
    buffer.push_back(unicode);
  }

void state_StringIn::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void state_StringIn::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void state_StringIn::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}