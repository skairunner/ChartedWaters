#include "State_prompt.h"

State_Prompt::State_Prompt(const int& w, const int& h, const std::string& message, bool& result)
  : msg(message), width(w), height(h), refToResult(result)
  {
  console = new TCODConsole(w, h);
  }

void State_Prompt::redraw()
  {
  console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->setDefaultBackground(TCODColor::black);
  console->printRect(1, 1, width-2, height-3, msg.c_str());
  console->print(1, height-1, "Press enter for yes, escape for no.");
  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, width, height, false);
  }

void State_Prompt::End()
  {
  delete console;
  }

bool State_Prompt::Init()
  {
  redraw();
  return true;
  }

void State_Prompt::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_RETURN)
    {
    refToResult = true;
    popMe = true;
    }
  else if (key == SDLK_ESCAPE)
    {
    refToResult = false;
    popMe = true;
    }
  }

void State_Prompt::Render(TCODConsole *root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0, 1.0f, 1.0f);
  }