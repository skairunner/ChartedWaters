#include "State_ShipStatus.h"

class State_shipPartInventory
{
public:
    State_shipPartInventory(Fleet& fleet);
    bool Init();
    void Update();
    void KeyUp(const int &key, const int &unicode);
    void KeyDown(const int &key, const int &unicode);

private:
    int selected = -1;
    bool pushedSomething = false;

    Fleet& refToFleet;
    Player& player;
    std::vector<ShipPart*> partList;
};

class State_selectShip : public State_ShipStatus
{
public:
    State_selectShip(Fleet& fleet, int& out);
private:
    int& pageit;
};