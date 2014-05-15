#pragma once
#include <string>
#include "goods.h"
#include "Ship.h"


// A fleet is multiple ships all acting as one! They let the player cheaply and effectively increase their storage and power.
// The speed is that of the slowest ship.
class Fleet
{
public:
    Fleet();
	void setName(const std::string& newName);
	std::string getName();
	std::string getType();
	bool addMoney(const int& amount); // false if the total money would go under 0.
	void addItem(const Item& item, const int& numberOf, const int& averagePrice);
	bool removeItem(const std::string& ItemID, const int& numberOf); // return false in case there are not enough items to remove.

    void changeShip(ShipPrototype& newship, int index); // Change the ship of the i'th index to the newship. If the index does not exist, create a new ship. If the index is larger than 4 or smaller than 0, throw a logic_exception.
    Ship& refShip(int index);
    int& refSailors(int index);
    void addRations(int number, int index = -1); // index being <0 means autofill from the first ship. Otherwise it means put all rations in one ship.
    void addSailors(int number, int training, int index = -1); // index of <0 means put sailors where needed, then put them in order from the first. Otherwise, put them all into the index ship.
    

    int getRations(); // sum of all rations
    int getNumSailors();
    bool hasEnoughSailors(); // if any one of the ships don't have enough sailors, false.


	int getEstimatedRationsNeeded();
	int getETA();
	int getNumberOfItems(const std::string& ID);
	int getPurchasePriceOf(const std::string& ID);
	int getShipPrice();
	int getMoney();
	int getTotalGoods();
	int getMaxGoods();
	int getTotalStorageUsed(); // cannons + sailors + goods
	int getMaxStorage();
	int getMaxCannons();
	int getCannons();
	int getMinSailors();
	int getMaxSailors();
	int getWaveResistance(); // Lowest WR of the entire fleet.
	int getMaxDurability();
	int getArmor();
	int getLateen();
	int getSquare();
	double getSpeed();
	int getBaseSpeed();
	int getMovementCounters(); // This one keeps the decimals. Used for movement, not stat display. Alters the class.
	std::string getDescription();
	int getTurning();
	std::string getSize();


	std::vector<LedgerItemTuple> returnListOfItems();
	std::vector<std::pair<LedgerItemTuple, int>> returnListOfItemsPerShip();

	void setPosition(const std::pair<int, int>& newPos);
	void setPath(const std::vector<std::pair<int, int>>& ppath);
	void updatePos(); // move 1 step along the path
	virtual void step(); // run the simulations for 1 day

	std::pair<int, int> getPosition();

	int character; // default F
    int numShips();
	int getFatigue(); // divide by 10; is weighted average of entire fleet
	int sailorsDied;
	int getTraining(); // divide by 10; is weighted average of entire fleet
	int getDurability(); // Sum of entire fleet
	std::string lastVisitedCity;
	std::pair<int, int> lastVisitedCityCoords;

	bool starving;
	bool unpaid;
	bool wrecked;

	double movementcounter;

	std::vector<std::pair<int, int>> path;
	std::map<int, Ship> ships;

	bool invisible;

    Player captain;
protected:
	std::string fleetname;
	std::pair<int, int> position;
};