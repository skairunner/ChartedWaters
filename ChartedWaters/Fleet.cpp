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
    else
    {
        if (ships.find(index) == ships.end())
            throw std::out_of_range(("The index " + std::to_string(index) + " does not exist.").c_str());
        ships[index].rations += number; // slap every single ration into the single ship's hold.
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
    if (index < 0)
    {
        std::vector<int> indexes = getKeys(ships);

        for (auto it = indexes.begin(); it < indexes.end(); it++)
        {
            if (number <= 0)
                break;
            Ship& ship = ships[*it];
            int neededSailors = ship.getMinSailors() - ship.sailors;
            if (neededSailors > 0 && neededSailors < number)
            {
                number -= neededSailors;
                ship.addSailors(neededSailors, training);
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
                int neededSailors = ship.getMaxSailors() - ship.sailors;
                if (neededSailors > 0 && neededSailors < number)
                {
                    number -= neededSailors;
                    ship.addSailors(neededSailors, training);
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
    return sum / total; // Throw away remainders!
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
    return lowest;
}

int Fleet::getETA()
{
    if (getSpeed() < 1)
        return 99999999;
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