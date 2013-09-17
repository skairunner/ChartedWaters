#include "Ship.h"
#include <map>
#include "town.h"
#include <boost/random/mersenne_twister.hpp>
#include "A_star.h"

typedef std::pair<int, int> coord;

class AIShip : public Ship
  {
  public:
    AIShip();
    void think(Pather& pather, std::map<coord, Town>& cityList, Town& currentTown = nullTown);
    std::map<coord, bool> cityList;
    void initItemDB(std::map<coord, Town>& world);

  private:
    enum STATES {STATE_ERROR = -1, STATE_WAIT = 0, STATE_PLOT, STATE_RESTOCK, STATE_MOVE, STATE_STARTING_WAIT, STATE_PURCHASE, STATE_INIT};
    boost::random::mt19937 gen;

    int random(const int& min, const int& max);
    void wait();
    void startingWait();
    void plot(Pather& pather, std::map<coord, Town>& cityList);
    void restock(Town& currentTown);
    void purchase(Town& currentTown);
    void DoMerchantLogic(Town& currentTown);
    void move();

    void updateDB(Town& town);
    
    std::map<std::string, MemoryItem> ItemDB;
    std::vector<std::string> masterItemList;
    MemoryItem& getItemFromDB(std::string& ID);

    static Town nullTown;
    int state;
    int timer;
    long unsigned int date;
    
  };