#include "AI_ship.h"
#include <random>
#include <vector>

class Battle;

class ShipCounter
  {
  friend class Battle;
  public:
    ShipCounter(AIShip& ref);
    enum StateOfMind {stateNONE = 0, stateFIGHT, stateFLIGHT};
    AIShip& refToShip;
    void step();


    std::vector<int> history;
    std::vector<int> HPHistory;

  private:
    void seek();

    int cannonTimer;
    int fightOrFlee;
    int velocity;
    int acceleration;
    int maxSpeed;
    int position;
    int distance; // vector pointing to other ship
    int side; // +/-



    int pen, damage, range;
  };


class Battle
  {
  public:
    Battle(AIShip& ship1, AIShip& ship2);
    void step();
    bool done;
    
    void print(std::string filename = std::string("output.txt"));

  private:
    int getDamage(ShipCounter& sc1, ShipCounter& sc2);
    void shootEachOther();
    ShipCounter s1, s2;
    void fightOrFlightHeuristic();
    void runAwayHeuristic();
    std::mt19937 gen;
  };