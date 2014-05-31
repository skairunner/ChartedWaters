#include "Fleet.h"
#include "utility.h"
#include <limits> 

Fleet::Fleet()
: character('F'), movementcounter(0), wrecked(false), invisible(false)
{

}

std::string Fleet::getName()
{
    return fleetname;
}

void Fleet::setName(const std::string& newName)
{
    fleetname = newName;
}

void Fleet::setShipName(const std::string& newName, int index)
{
    if (index < 0 || index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (ships.find(index) == ships.end())
    {
        throw std::out_of_range(("Ship index " + std::to_string(index) + "does not exist.").c_str());
    }
    ships[index].setName(newName);
}

coord Fleet::getPosition()
{
    return position;
}

void Fleet::setPosition(const coord& newpos)
{
    position = newpos;
}

bool Fleet::addMoney(const int& amount)
{
    if (captain.ducats + amount < 0)
        return false;
    captain.ducats += amount;
    return true;
}

void Fleet::changeShip(ShipPrototype& newship, int index)
{
    if (index < 0 || index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (ships.find(index) == ships.end())
    {
        ships[index] = Ship(newship);
    }
    else
        ships[index].changeShip(newship);
}

Ship& Fleet::refShip(int index)
{
    if (index < 0 || index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (ships.find(index) == ships.end())
        throw std::out_of_range(("The index " + std::to_string(index) + " does not exist.").c_str());
    return ships[index];
}

int& Fleet::refSailors(int index)
{
    if (index < 0 || index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (ships.find(index) == ships.end())
        throw std::out_of_range(("The index " + std::to_string(index) + " does not exist.").c_str());
    return ships[index].sailors;
}

void Fleet::addRations(int number, int index)
{
    if (index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (index < 0)
    {
        if (number >= 0)
        {
            std::vector<int> indexes = getKeys(ships);
            int iteration = 0;
            while (number > 0)
            {
                Ship& ship = ships[indexes[iteration]];
                int leftoverSpace = ship.getMaxGoods() - ship.getTotalGoods();
                leftoverSpace *= 10; // because 1 cargo unit of rations is actually 10 rations.
                if (leftoverSpace > number)
                {
                    ship.rations += number;
                    number = 0;
                }
                else
                {
                    number -= leftoverSpace;
                    ship.rations += leftoverSpace;
                }
                iteration++;
            }
            ships[indexes.back()].rations += number; // Just stuff all the leftover rations into the final ship :P
        }
        else // When removing rations, a slightly different method must be used.
        {
            std::vector<int> indexes = getKeys(ships);
            int iteration = 0;
            while (number > 0 && (size_t)iteration < indexes.size())
            {
                Ship& ship = ships[indexes[iteration]];
                int rationsRemovable = ship.rations;
                if (rationsRemovable > -number)
                {
                    ship.rations += number;
                    number = 0;
                }
                else // If I can't remove as many rations needed, subtract as much as possible and move on
                {
                    number += rationsRemovable;
                    ship.rations = 0;
                }
                iteration++;
            }
            // Doesn't matter whether I haven't removed all the rations needed.
        }
    }
    else
    {
        if (ships.find(index) == ships.end())
            throw std::out_of_range(("The index " + std::to_string(index) + " does not exist.").c_str());
        Ship& ship = ships[index];
        if (number >= 0)
            ship.rations += number; // slap every single ration into the single ship's hold.
        else
        {
            if (ship.rations > -number)
            {
                ship.rations += number;
                number = 0;
            }
            else
                ship.rations = 0;
        }
    }
}

void Fleet::setPath(const std::vector<std::pair<int, int>>& ppath)
{
    if (ppath.size() > 0)
    {
        path.clear();
        path.reserve(ppath.size());

        for (auto it = ppath.rbegin(); it < ppath.rend() - 1; it++) // Skip the last, as it's 0,0.
        {
            path.push_back(*it);
        }
    }
}

void Fleet::addSailors(int number, int training, int index)
{
    if (index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (number >= 0)
    {
        if (index < 0)
        {
            std::vector<int> indexes = getKeys(ships);

            for (auto it = indexes.begin(); it < indexes.end(); it++)
            {
                if (number <= 0)
                    break;
                Ship& ship = ships[*it];
                int neededSailors = ship.getMinSailors() - ship.sailors;
                if (neededSailors > 0 && neededSailors <= number)
                {
                    number -= neededSailors;
                    ship.addSailors(neededSailors, training);
                }
                else if (neededSailors > 0 && neededSailors > number)
                {
                    ship.addSailors(number, training);
                    number = 0;
                }
            }
            if (number > 0) // oh no sailors are left over!
            {
                // fear not! just put them into ships that have left over spaces.
                for (auto it = indexes.begin(); it < indexes.end(); it++)
                {
                    if (number <= 0)
                        break;
                    Ship& ship = ships[*it];
                    int sailorSpots = ship.getMaxSailors() - ship.sailors;
                    if (sailorSpots > 0 && sailorSpots <= number)
                    {
                        number -= sailorSpots;
                        ship.addSailors(sailorSpots, training);
                    }
                    else if (sailorSpots > 0 && sailorSpots > number)
                    {
                        ship.addSailors(number, training);
                        number = 0;
                    }
                }

                if (number > 0) // wait, there's still sailors left?
                    ships[indexes[0]].addSailors(number, training); // give them all to the first ship and let the player sort out the issues.
            }
        }
        else
        {
            ships[index].addSailors(number, training);
        }
    }
    else // Dealing with negative numbers means we're removing sailors.
    {
        std::vector<int> indexes = getKeys(ships);
        int iteration = 0;
        // First trim off not necessarily needed sailors
        while (number > 0 && (size_t)iteration < indexes.size())
        {
            Ship& ship = ships[indexes[iteration]];
            int removable = ship.sailors - ship.getMinSailors();
            if (removable <= 0)
                continue;
            if (removable > -number)
            {
                ship.addSailors(number, 0);
                number = 0;
            }
            else
            {
                number += removable;
                ship.addSailors(-removable, 0);
            }
            iteration++;
        }

        iteration = 0;
        // If we can't remove enough sailors without harming ships' abilities, do it.
        while (number > 0 && (size_t)iteration < indexes.size())
        {
            Ship& ship = ships[indexes[iteration]];
            int removable = ship.sailors;
            if (removable <= 0)
                continue;
            if (removable > -number)
            {
                ship.addSailors(number, 0);
                number = 0;
            }
            else
            {
                number += removable;
                ship.addSailors(-removable, 0);
            }
            iteration++;
        }
    }
}

int Fleet::getFatigue()
{
    auto keys = getKeys(ships);
    int total = 0; // sum of weight
    int sum = 0; // sum of weight * fatigue
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        Ship& ship = ships[*it];
        total += ship.sailors;
        sum += ship.sailors * ship.fatigue;
    }

    if (total == 0)
        return 1000;
    return sum / total; // Throw away remainders!
}

void Fleet::removeFatigue(int number, int index)
{
    if (index > 4)
        throw std::out_of_range("Ship index out of range: must be [0, 4]");
    if (index < 0)
    {
        std::vector<int> indexes = getKeys(ships);

        for (auto it = indexes.begin(); it < indexes.end(); it++)
        {
            ships[*it].fatigue -= number;
            if (ships[*it].fatigue < 0)
                ships[*it].fatigue = 0;
        }
    }
    else
    {
        ships[index].fatigue -= number;
        if (ships[index].fatigue < 0)
            ships[index].fatigue = 0;
    }
}


int Fleet::getMoney()
{
    return captain.ducats;
}

int Fleet::getTotalGoods()
{
    int total = 0;
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        total += it->second.getTotalGoods();
    }
    return total;
}

int Fleet::getMaxGoods()
{
    int total = 0;
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        total += it->second.getMaxGoods();
    }
    return total;
}

bool Fleet::removeItem(const std::string& ItemID, int numberOf)
{
    // First, count how many in total we have.
    int count = getNumberOfItems(ItemID);
    if (count < numberOf)
        return false;

    // Well, now that we know we have enough items to remove, let's set about doing it.
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        int num = it->second.getNumberOfItems(ItemID);
        if (num == 0)
            continue;
        if (num > numberOf)
        {
            it->second.removeItem(ItemID, numberOf);
            return true;
        }            
        else
        {
            it->second.removeItem(ItemID, num);
            numberOf -= num;
        }
    }
    return true;
}

int Fleet::getPurchasePriceOf(const std::string& ID)
{
    // Must recalculate the purchase price.
    int totalPrice = 0;
    int total = 0;
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        Ship& ship = it->second;
        int price = ship.getPurchasePriceOf(ID);
        int number = ship.getNumberOfItems(ID);
        totalPrice += number * price;
        total += number;
    }

    return totalPrice / total;
}

void Fleet::addItem(const Item& item, int numberOf, const int& averagePrice)
{
    // First try finding an existing item
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        Ship& ship = it->second;
        // If a ship already has the item,
        if (ship.getNumberOfItems(item.ID) > 0)
        {
            //  and it can hold all of the items.
            if (ship.getTotalGoods() + numberOf <= ship.getMaxGoods())
            {
                ship.addItem(item, numberOf, averagePrice);
                return;
            }                
            else // and it cannot hold -all- of the items
            {
                int possible = ship.getMaxGoods() - ship.getTotalGoods();
                ship.addItem(item, possible, averagePrice);
                numberOf -= possible;
            }
        }
    }

    // If none of the ships already have the item, or the ships that do can't hold all of them
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        Ship& ship = it->second;
        if (ship.getTotalGoods() >= ship.getMaxGoods())
            continue;
        int possible = ship.getMaxGoods() - ship.getTotalGoods();
        if (possible >= numberOf)
        {
            ship.addItem(item, numberOf, averagePrice);
            return;
        }
        else // not enough space?
        {
            ship.addItem(item, possible, averagePrice);
            numberOf -= possible;
        }
    }

    // If after all this there are still items left, put it all on the first ship and let the player deal with it.
    ships.begin()->second.addItem(item, numberOf, averagePrice);
}

int Fleet::getRations()
{
    auto keys = getKeys(ships);
    int num = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        num += ships[*it].rations;
    }
    return num;
}

int Fleet::takeRations(const int& number)
{
    auto keys = getKeys(ships);
    int taken = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        if (ships[*it].rations + taken > number) // If the first ship has enough rations
        {
            ships[*it].rations -= (number - taken); // Fill up taken, and subtract the amount
            taken = number;
            break; // No need to continue.
        }
        else // the ship's supply of rations don't satisfy our needs
        {
            taken += ships[*it].rations;
            ships[*it].rations = 0;
        }
    }

    return taken;
}

int Fleet::numShips()
{
    return ships.size();
}

int Fleet::getDurability()
{
    auto keys = getKeys(ships);
    int num = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        num += ships[*it].durability;
    }
    return num;
}

int Fleet::getMaxDurability()
{
    auto keys = getKeys(ships);
    int num = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        num += ships[*it].getMaxDurability();
    }
    return num;
}

int Fleet::getNumSailors()
{
    auto keys = getKeys(ships);
    int num = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        num += ships[*it].sailors;
    }
    return num;
}

int Fleet::getMinSailors()
{
    auto keys = getKeys(ships);
    int num = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        num += ships[*it].getMinSailors();
    }
    return num;
}

int Fleet::getMaxSailors()
{
    auto keys = getKeys(ships);
    int num = 0;
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        num += ships[*it].getMaxSailors();
    }
    return num;
}

bool Fleet::hasEnoughSailors()
{
    auto keys = getKeys(ships);
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        Ship& ship = ships[*it];
        if (ship.sailors < ship.getMinSailors())
            return false;
    }
    return true;
}

bool Fleet::isLoadedProperly()
{
    auto keys = getKeys(ships);
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        Ship& ship = ships[*it];
        if (ship.getMaxStorage() < ship.getTotalStorageUsed())
            return false;
    }
    return true;
}

int Fleet::getEstimatedRationsNeeded()
{
    return getETA() * getNumSailors();
}


double Fleet::getSpeed()
{
    auto keys = getKeys(ships);
    double lowest = std::numeric_limits<double>::max();
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        double speed = ships[*it].getSpeed();
        if (speed < lowest)
            lowest = speed;
    }
    return lowest;
}

int Fleet::getWaveResistance()
{
    auto keys = getKeys(ships);
    double lowest = std::numeric_limits<double>::max();
    for (auto it = keys.begin(); it < keys.end(); it++)
    {
        int waveResistance = ships[*it].waveResistance;
        if (waveResistance < lowest)
            lowest = waveResistance;
    }
    return int(lowest);
}

int Fleet::getNumberOfItems(const std::string& ItemID)
{
    // Gotta sum the entire fleet.
    int count = 0;
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        count += it->second.getNumberOfItems(ItemID);
    }
    return count;
}

int Fleet::getETA()
{
    if (getSpeed() < 1)
        return 0;
    return (int)(ceil(path.size() / floor((double)getSpeed())));;
}

int Fleet::getMovementCounters()
{
    movementcounter += getSpeed();
    int used = (int)movementcounter;
    movementcounter -= used;
    return used;
}

void Fleet::updatePos()
{
    if (path.size() > 0){
        setPosition(path.back());
        path.pop_back();

        for (auto it = ships.begin(); it != ships.end(); it++)
        {
            it->second.setPosition(position);
        }
    }
}

void Fleet::step()
{
    starving = false;
    wrecked = false;

    if (position != lastVisitedCityCoords)
    {
        for (auto it = ships.begin(); it != ships.end(); it++)
        {
            it->second.fleetStep(this);
            if (it->second.starving)
                starving = true;
            if (it->second.wrecked)
                wrecked = true;
        }
    }
}

std::vector<LedgerItemTuple> Fleet::returnListOfItems()
{
    std::vector<LedgerItemTuple> items;
    for (auto it = ships.begin(); it != ships.end(); it++)
    {
        Ship& s = it->second;
        auto shipItems = s.returnListOfItems();
        items.reserve(items.size() + std::distance(shipItems.begin(), shipItems.end()));
        items.insert(items.end(), shipItems.begin(), shipItems.end()); // basically Python's extend() method.
    }

    // The problem is that some items may be duplicated. Must consolidate them.
    std::map<std::string, LedgerItemTuple> items2;
    for (auto it = items.begin(); it != items.end(); it++)
    {
        if (items2.find(it->itemID) == items2.end())
            items2[it->itemID] = *it;
        else
        {
            auto& listing = items2[it->itemID]; // The consolidated one
            int total = listing.averagePurchasePrice * listing.numberOfItems + it->averagePurchasePrice * it->numberOfItems;
            int newaverage = total / (listing.numberOfItems + it->numberOfItems); // This is slightly inaccurate, but it does not matter that much.
            listing.averagePurchasePrice = newaverage;
            listing.numberOfItems += it->numberOfItems;
        }
    }

    items.clear();
    for (auto it = items2.begin(); it != items2.end(); it++)
        items.push_back(it->second);

    return items;
}