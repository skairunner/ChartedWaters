#include "State_shipPartShop.h"

class State_shipPartInventory : public State_shipPartShop
  {
  public:
    State_shipPartInventory(Ship& ship);
    bool Init();
    void Update();
  private:
    std::vector<ShipPart*> partList;
  };