#include "GameState.h"
#include "Fleet.h"
#include <vector>

class State_equipParts : public GameState
{
public:
    State_equipParts(Fleet& fleet);

    virtual bool Init(){ return true; }

    virtual void Update();
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
    int endOfText;
    int page;
    int pageit;
    std::vector<int> pages;
    Fleet& refToFleet;

    int printStats();
    void updateLookup(); // update partsLookup.
    TCODConsole* console;

    int selector;
    int selectorStart; // Y coordinates from which the selector starts.
    std::vector<std::pair<char, int>> partsLookup; // What value of selector corresponds to which part.

    bool waiting;
    bool doRedraw;
    void redraw();
};

class State_pickPartAction : public GameState
{
public:
    State_pickPartAction(Fleet& fleet, const int& shipnum, const char& c, const int& i, const int& Ypos); // c, i are from partLookup in equipParts. Ypos is where to start drawing the window on the root console.
    virtual bool Init(){ return true; }

    virtual void Update();
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
    TCODConsole* console;
    Fleet& refToFleet;
    char type;
    std::string type_name;
    int index;
    int page;
    int YPos;
    bool hasParts(); // The player is in possession of at least 1 part of <type>

    int pickedIndex;
    bool waiting; // waiting for selectPart to exit.
};

class State_selectPart : public GameState
{
public:
    State_selectPart(Fleet& fleet, const int& shipnum, const char& type, int& resultsave);
    void Render(TCODConsole* root);
    void End();
    virtual void KeyDown(const int &key, const int &unicode);

private:
    void redraw();
    void updateStats();

    void getProfile(ShipCannons& item, int line);
    void getProfile(ShipSails& item, int line);
    void getProfile(ShipArmor& item, int line);

    int getInventorySize();

    TCODConsole* console;
    TCODConsole* partConsole;
    int selector;
    int page;
    int& resultRef; 
    char parttype;
    Fleet& refToFleet;
};