#include "State_showskills.h"
#include "utility.h"
#include <SDL.h>

State_ShowSkills::State_ShowSkills(Player& captain)
:player(captain)
{
    window = new TCODConsole(screenwidth / 2, screenheight / 2);
    w = screenwidth / 2;
    h = screenheight / 2;
}

bool State_ShowSkills::Init()
{
    redraw();
    return true;
}

void State_ShowSkills::redraw()
{
    window->setDefaultBackground(TCODColor::black);
    window->setDefaultForeground(DarkMabinogiBrown);
    window->printFrame(0, 0, w, h, true, TCOD_BKGND_SET);
    
    auto skills = player.skills;

    int line = 1; // current line.
    window->setDefaultForeground(TCODColor::grey);
    window->print(1, line++ , "Skills are automatically trained.");
    line += window->printRect(1, line, w-2, 0, "They represent the general merchant-y skill for each category of goods. Each level in a category reduces the buy price by 1% and increases the sell price by 1%. Levels in categories also increase the number of goods available at one time. Or they should, I might not have done that yet.");
    window->setDefaultForeground(TCODColor::white);
    line++;

    for (auto it = skills.begin(); it != skills.end(); it++)
    {
        if (it->first == "")
            continue;
        std::string name = it->first.substr(0, 7);
        if (name.size() < 7)
        for (int i = 0; i <= 7 - name.size(); i++)
            name += " ";
        int level = it->second.getLevel();
        int points = it->second.netPointsNeeded();
        double percentToNext = it->second.getValue() - level;
        window->print(1, line++, "%s    lv%d   %d xp needed (%.1f%%)", name.c_str(), level, points, percentToNext * 100);
    }
}

State_ShowSkills::~State_ShowSkills()
{
    if (window)
        delete window;
}

void State_ShowSkills::End()
{
    if (window)
        delete window;
}



void State_ShowSkills::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_ESCAPE)
        popMe = true;
}

void State_ShowSkills::Render(TCODConsole *root)
{
    TCODConsole::blit(window, 0, 0, 0, 0, root, screenwidth * 1 / 4, screenheight * 1 / 4);
}