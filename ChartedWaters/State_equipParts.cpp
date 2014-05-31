#include "State_equipParts.h"
#include "utility.h"
#include <SDL.h>




State_equipParts::State_equipParts(Fleet& fleet)
: refToFleet(fleet), selector(-1)
{
    popMe = false;
    console = new TCODConsole(screenwidth, screenheight);
    pageit = 0;
    pages = getKeys(fleet.ships);
    page = pages[pageit];
    doRedraw = true;
    waiting = false;
    updateLookup();
}

void State_equipParts::End()
{
    if (console)
        delete console;
}


int State_equipParts::printStats()
{
    Ship& current = refToFleet.ships[page];

    int line = 1;

    console->setColorControl(TCOD_COLCTRL_1, TCODColor::grey, TCODColor::black);
    console->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);
    console->setColorControl(TCOD_COLCTRL_3, TCODColor::lighterYellow, TCODColor::black);
    TCOD_colctrl_t typeToUse = (TCOD_colctrl_t)8;

    console->setDefaultForeground(TCODColor::white);
    console->print(1, line++, ("The " + current.getType() + " " + current.getName()).c_str()); swapLineColors(console, line);
    console->print(1, line++, "Type : %s", current.getType().c_str()); swapLineColors(console, line);
    console->print(1, line++, "Size : %s", current.getSize().c_str()); swapLineColors(console, line);
    console->print(1, line++, "Total storage: %d", current.getMaxStorage()); swapLineColors(console, line);
    console->print(1, line++, "Goods: %d/%d    Sailors: %d%c(%d)%c/%d    Cannons: %d%c/%d", current.getTotalGoods(), current.getMaxGoods(),
        current.sailors, TCOD_COLCTRL_1, current.getMinSailors(), TCOD_COLCTRL_STOP, current.getMaxSailors(),
        current.getCannons(), TCOD_COLCTRL_1, current.getMaxCannons()); swapLineColors(console, line);

    console->print(1, line++, "Lateen sails: %d+%d    Square sails: %d+%d", current.getBaseLateen(), current.getAddedLateen(), current.getBaseSquare(), current.getAddedSquare()); swapLineColors(console, line);
    console->print(1, line++, "Speed: %.1f%c/%d.0", current.getSpeed(), TCOD_COLCTRL_1, current.getBaseSpeed()); swapLineColors(console, line);
    console->print(1, line++, "Wave resistance: %d", current.getWaveResistance()); swapLineColors(console, line);

    int addedTurning = current.getAddedTurning();
    if (addedTurning >= 0)
        console->print(1, line++, "Turning: %d+%d", current.getBaseTurning(), addedTurning);
    else
        console->print(1, line++, "Turning: %d%d", current.getBaseTurning(), addedTurning); // negative means it'll have a sign.
    swapLineColors(console, line);

    console->print(1, line++, "Armor: %d+%d", current.getBaseArmor(), current.getAddedArmor()); swapLineColors(console, line);
    line++;

    if (current.fatigue > 900)
        typeToUse = (TCOD_colctrl_t)2;
    else if (current.fatigue > 500)
        typeToUse = (TCOD_colctrl_t)3;

    console->print(1, line++, "Fatigue: %c%.1f%c/100%c", typeToUse, current.fatigue / 10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
    console->print(1, line++, "Training: %.1f%c/100%c", current.training / 10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

    typeToUse = (TCOD_colctrl_t)8;
    if (current.rations < 50)
        typeToUse = (TCOD_colctrl_t)2;

    console->print(1, line++, "Rations: %c%.1f%c", typeToUse, current.rations / 10.0f, TCOD_COLCTRL_STOP);

    typeToUse = (TCOD_colctrl_t)8;
    if (current.durability / (double)current.getMaxDurability() < 0.1)
        typeToUse = (TCOD_colctrl_t)2;
    else if (current.durability / (double)current.getMaxDurability() < 0.5)
        typeToUse = (TCOD_colctrl_t)3;
    console->print(1, line++, "Durability: %c%d%c/%d", typeToUse, current.durability, TCOD_COLCTRL_STOP, current.getMaxDurability()); swapLineColors(console, line);

    return line;
}

void State_equipParts::updateLookup()
{
    Ship& current = refToFleet.ships[page];
    partsLookup.clear();
    std::vector<int>& cannons = vectornumbers::indexes[current.getCannonSlots()];
    std::vector<int>& armors = vectornumbers::indexes[current.getArmorSlots()];
    std::vector<int>& sails = vectornumbers::indexes[current.getSailSlots()];
    for (int i : cannons)
        partsLookup.push_back(std::pair<char, int>('c', i));
    for (int i : armors)
        partsLookup.push_back(std::pair<char, int>('a', i));
    for (int i : sails)
        partsLookup.push_back(std::pair<char, int>('s', i));
    selector = -1;
}

void State_equipParts::redraw()
{
    console->clear();
    int startline = printStats();
    selectorStart = startline + 1;
    startline = printEquipment(console, refToFleet.ships[page], startline + 1);    
    drawPageDots(console, 1, startline + 1, pageit, refToFleet.ships.size());
    console->setDefaultForeground(TCODColor(96, 71, 64));
    console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
    if (selector != -1)
        invertLine(console, selector + selectorStart);
    doRedraw = false;
    endOfText = startline+1;
}

void State_equipParts::Update()
{
    if (waiting)
    {
        waiting = false;
        doRedraw = true;
    }
    if (doRedraw)
        redraw();
}

void State_equipParts::Render(TCODConsole *root)
{
    TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.9f);
}

void State_equipParts::KeyDown(const int &key, const int &unicode)
{
    switch (key)
    {
    case SDLK_ESCAPE:
        popMe = true;
        break;
    case SDLK_LEFT:
        if (pageit > 0)
        {
            pageit--;
            page = pages[pageit];
            doRedraw = true;
            updateLookup();
        }
        break;
    case SDLK_RIGHT:
        if (pageit < int(refToFleet.ships.size()) - 1)
        {
            pageit++;
            page = pages[pageit];
            doRedraw = true;
            updateLookup();
        }
        break;
    case SDLK_UP:
        if (selector > 0)
        {
            selector--;
            doRedraw = true;
        }
        break;
    case SDLK_DOWN:
        if (selector < (int)partsLookup.size() - 1)
        {
            selector++;
            doRedraw = true;
        }
        break;
    case SDLK_RETURN:
        if (selector != -1)
        {
            nextState = new State_pickPartAction(refToFleet, page, partsLookup[selector].first, partsLookup[selector].second, endOfText);
            waiting = true;
            pushSomething = true;
        }
        break;
    default:
        break;
    }
}

////////
////////
////////

State_pickPartAction::State_pickPartAction(Fleet& fleet, const int& shipnum, const char& c, const int& i, const int& Ypos)
: refToFleet(fleet) 
{
    page = shipnum;
    type = c;
    if (type == 'c')
        type_name = "cannons";
    else if (type == 'a')
        type_name = "armor";
    else if (type == 's')
        type_name = "sails";

    index = i;
    console = new TCODConsole(screenwidth, screenheight);
    YPos = Ypos;

    waiting = false;

    redraw();
}

void State_pickPartAction::End()
{
    if (console)
        delete console;
}

void State_pickPartAction::Render(TCODConsole* root)
{
    TCODConsole::blit(console, 0, 0, 0, 0, root, 0, YPos);
}

void State_pickPartAction::redraw()
{
    Ship& current = refToFleet.ships[page];
    
    console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
    int line = 1;
    console->setDefaultForeground(TCODColor::darkerYellow);

    std::string equippedName;
    if (type == 'c')
    {
        auto result = current.cannonList.find(index);
        if (result == current.cannonList.end())
            equippedName = "nothing";
        else
        {
            ShipCannons& sc = result->second;
            equippedName = sc.shopName();
        }
    }
    else if (type == 'a')
    {
        auto result = current.armorList.find(index);
        if (result == current.armorList.end())
            equippedName = "nothing";
        else
        {
            ShipArmor& sa = result->second;
            equippedName = sa.shopName();
        }
    }
    else if (type == 's')
    {
        auto result = current.sailList.find(index);
        if (result == current.sailList.end())
            equippedName = "nothing";
        else
        {
            ShipSails& ss = result->second;
            equippedName = ss.shopName();
        }
    }

    console->print(1, line++, "Equipped: %s", equippedName.c_str());
    line++;
    console->setDefaultForeground(TCODColor::white);

    if (hasParts()) // Disable equipping if there are no parts to equip!
        console->print(1, line++, "%cE%cquip %s", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, type_name.c_str());
    else
    {
        console->setDefaultForeground(TCODColor::lightGrey);
        console->print(1, line++, "Equip %s %cYou do not have any %s to equip%c", type_name.c_str(), TCOD_COLCTRL_1, type_name.c_str(), TCOD_COLCTRL_STOP);
    }
        
    if (equippedName == "nothing")
    {
        console->setDefaultForeground(TCODColor::lightGrey);
        console->print(1, line++, "Unequip %s", type_name.c_str());
    }
    else
        console->print(1, line++, "%cU%cnequip %s", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, type_name.c_str());

    console->setDefaultForeground(MabinogiBrown);
    console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
}

void State_pickPartAction::Update()
{
    if (waiting && pickedIndex != -5) // -5 is the arbitrary number it is initalized with. fail state for selectPart returns -1.
    {
        if (pickedIndex != -1)
        {
            Ship& current = refToFleet.ships[page];
            if (type == 'c')
            {
                ShipCannons& sc = refToFleet.captain.cannonInventory[pickedIndex];
                int e = refToFleet.ships[page].addCannons(index, sc);
                if (e == shipSUCCESS) // There was no existing part.
                    removeInPlace(pickedIndex, refToFleet.captain.cannonInventory);
                // If e == shipSWAPPED there is no need to do anything, because sc was a reference and the contents of the inventory are automatically updated.
            }
            else if (type == 'a')
            {
                ShipArmor sa = refToFleet.captain.armorInventory[pickedIndex];
                int e = refToFleet.ships[page].addArmor(index, sa);
                if (e == shipSUCCESS) // There was no existing part.
                    removeInPlace(pickedIndex, refToFleet.captain.armorInventory);
                // If e == shipSWAPPED there is no need to do anything, because sc was a reference and the contents of the inventory are automatically updated.
            }
            else if (type == 's')
            {
                ShipSails ss = refToFleet.captain.sailInventory[pickedIndex];
                int e = refToFleet.ships[page].addSail(index, ss);
                if (e == shipSUCCESS) // There was no existing part.
                    removeInPlace(pickedIndex, refToFleet.captain.sailInventory);
                // If e == shipSWAPPED there is no need to do anything, because sc was a reference and the contents of the inventory are automatically updated.
            }
        }

        waiting = false;
        popMe = true;
    }
}

void State_pickPartAction::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_ESCAPE)
        popMe = true;
    else if (key == SDLK_u) // unequip
    {
        // Check if there is something.
        Ship& current = refToFleet.ships[page];
        if (type == 'c')
        {
            auto result = current.cannonList.find(index);
            if (result != current.cannonList.end())
            {
                ShipCannons sc = result->second;
                int e = refToFleet.ships[page].removeCannons(index, sc);
                if (e != 0)
                    refToFleet.captain.cannonInventory.push_back(sc);
            }
        }
        else if (type == 'a')
        {
            auto result = current.armorList.find(index);
            if (result != current.armorList.end())
            {
                ShipArmor sa = result->second;
                int e = refToFleet.ships[page].removeArmor(index, sa);
                if (e != 0)
                    refToFleet.captain.armorInventory.push_back(sa);
            }
        }
        else if (type == 's')
        {
            auto result = current.sailList.find(index);
            if (result != current.sailList.end())
            {
                ShipSails ss = result->second;
                int e = refToFleet.ships[page].removeSail(index, ss);
                if (e != 0) // did not fail
                    refToFleet.captain.sailInventory.push_back(ss);
            }
        }

        popMe = true;
    }
    else if (key == SDLK_e)
    {
        if (hasParts())
        {
            pickedIndex = -5;
            waiting = true;
            nextState = new State_selectPart(refToFleet, page, type, pickedIndex);
            pushSomething = true;
        }
    }
}


bool State_pickPartAction::hasParts()
{
    if (type == 'c')
    {
        if (refToFleet.captain.cannonInventory.size() > 0)
            return true;
        else
            return false;
    }
    else if (type == 'a')
    {
        if (refToFleet.captain.armorInventory.size() > 0)
            return true;
        else
            return false;
    }
    else if (type == 's')
    {
        if (refToFleet.captain.sailInventory.size() > 0)
            return true;
        else
            return false;
    }
    return false;
}
////////
////////
////////

State_selectPart::State_selectPart(Fleet& fleet, const int& shipnum, const char& type, int& resultsave)
: refToFleet(fleet), parttype(type), resultRef(resultsave), page(shipnum)
{
    selector = 0;
    console = new TCODConsole(screenwidth/2, screenheight);
    partConsole = new TCODConsole(screenwidth / 2, screenheight);
    redraw();
    updateStats();
}

void State_selectPart::End()
{
    if (console)
        delete console;
    if (partConsole)
        delete partConsole;
}

void State_selectPart::Render(TCODConsole* root)
{
    TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0);
    TCODConsole::blit(partConsole, 0, 0, 0, 0, root, screenwidth / 2, 0);
}

void State_selectPart::redraw()
{
    Player player = refToFleet.captain;
    int line = 1;

    console->clear();
    console->setDefaultForeground(TCODColor::white);
    console->print(1, line++, "");

    // First list all the relevant parts.
    if (parttype == 'c') // CANNONS
    {
        for (auto it = player.cannonInventory.begin(); it < player.cannonInventory.end(); it++)
        {
            console->print(1, line++, (it->shopName()).c_str()); // List the names            
        } 
    }
    else if (parttype == 'a') // ARMOR
    {
        for (auto it = player.armorInventory.begin(); it < player.armorInventory.end(); it++)
        {
            console->print(1, line++, (it->shopName()).c_str()); // List the names            
        }
    }
    else if (parttype == 's') // SAILS
    {
        for (auto it = player.sailInventory.begin(); it < player.sailInventory.end(); it++)
        {
            console->print(1, line++, (it->shopName()).c_str()); // List the names            
        }
    }

    invertLine(console, selector + 2);

    console->setDefaultForeground(MabinogiBrown);
    console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
}

void State_selectPart::updateStats()
{
    partConsole->clear();
    partConsole->setDefaultForeground(TCODColor::white);
    if (parttype == 'c')
        getProfile(refToFleet.captain.cannonInventory[selector], 1);
    if (parttype == 's')
        getProfile(refToFleet.captain.sailInventory[selector], 1);
    if (parttype == 'a')
        getProfile(refToFleet.captain.armorInventory[selector], 1);

    partConsole->setDefaultForeground(MabinogiBrown);
    partConsole->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
}

void State_selectPart::getProfile(ShipArmor& item, int line)
{
    swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Name : %s", item.name.c_str()); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Price : %d", item.price); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Armor : %d", item.armor); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Speed : %.2f", item.speed); swapLineColors(partConsole, line);
    line += 3;
    partConsole->setDefaultForeground(TCODColor::silver);
    int height = partConsole->printRect(1, line, 48, 0, "%s", item.desc.c_str());
    line += height; swapLineColors(partConsole, line);
}

void State_selectPart::getProfile(ShipSails& item, int line)
{
    swapLineColors(console, line);
    partConsole->print(1, line++, "Name : %s", item.name.c_str()); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Price : %d", item.price); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Lateen : +%d    Square : +%d", item.lateen, item.square); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Turning : %d", item.turning); swapLineColors(partConsole, line);
    line += 3;
    partConsole->setDefaultForeground(TCODColor::silver);
    int height = partConsole->printRect(1, line, 48, 0, "%s", item.desc.c_str());
    line += height; swapLineColors(partConsole, line);
}

void State_selectPart::getProfile(ShipCannons& item, int line)
{
    swapLineColors(console, line);
    partConsole->print(1, line++, "Name : %s", item.shopName().c_str()); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Price : %d", item.shopPrice()); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Damage : %d", item.getDamage()); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Penetration : %d", item.penetration); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Range : %d", item.range); swapLineColors(partConsole, line);
    partConsole->print(1, line++, "Reload speed : %d", item.reload); swapLineColors(partConsole, line);
    line += 3;
    partConsole->setDefaultForeground(TCODColor::silver);
    int height = partConsole->printRect(1, line, 48, 0, "%s", item.desc.c_str());
    line += height; swapLineColors(partConsole, line);
}

int State_selectPart::getInventorySize()
{
    int size = 0;
    if (parttype == 'c')
        size = refToFleet.captain.cannonInventory.size();
    if (parttype == 's')
        size = refToFleet.captain.sailInventory.size();
    if (parttype == 'a')
        size = refToFleet.captain.armorInventory.size();
    return size;
}

void State_selectPart::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_ESCAPE)
    {
        resultRef = -1;
        popMe = true;
    }
    else if (key == SDLK_DOWN)
    {
        int size = getInventorySize();
        if (selector < size - 1)
        {
            selector++;
            redraw();
            updateStats();
        }
    }
    else if (key == SDLK_UP)
    {
        int size = getInventorySize();
        if (selector > 0)
        {
            selector--;
            redraw();
            updateStats();
        }
    }
    else if (key == SDLK_RETURN)
    {
        resultRef = selector;
        popMe = true;
    }

}