#include "Volunteer.h"
#include "Customer.h"
#include <string>
#include <vector>
#include "Order.h"
using std::string;
using std::vector;

// constructor
Volunteer::Volunteer(int id, const string &name) : completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name){};

int Volunteer::getId() const
{
    return id;
}

const string &Volunteer::getName() const
{
    return name;
}

int Volunteer::getActiveOrderId() const
{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const
{
    return completedOrderId;
}

// Signal whether the volunteer is currently processing an order
bool Volunteer::isBusy() const
{
    return activeOrderId != NO_ORDER;
}

void Volunteer::updateCompleted()
{
    completedOrderId = NO_ORDER;
}

//__________________________________CollectorVolunteer_______________________________

// constructor
CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown) : Volunteer(id, name), coolDown(coolDown), timeLeft(-1){};

CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step()
{
    bool orderCompleted = decreaseCoolDown();
    if (orderCompleted)
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const
{
    return timeLeft;
}

// Decrease timeLeft by 1,return true if timeLeft=0,false otherwise
bool CollectorVolunteer::decreaseCoolDown()
{
    timeLeft = timeLeft - 1;
    if (timeLeft == 0)
        return true;
    else
    {
        return false;
    }
}

bool CollectorVolunteer::hasOrdersLeft() const
{
    return true;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy();
}

void CollectorVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    timeLeft = coolDown;
}

string CollectorVolunteer::toString() const
{
    if (isBusy())
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy: True" +
               "\n" + "OrderId:" + std::to_string(getActiveOrderId()) + "\n" + "timeLeft:" + std::to_string(getTimeLeft()) +
               "\n" + "ordersLeft: No Limit";
    }

    else
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy: False" +
               "\n" + "OrderId: None" + "\n" + "timeLeft: None" +
               "\n" + "ordersLeft: No Limit";
    }
}


void CollectorVolunteer::setTimeLeft(int newTimeLeft)
{
    timeLeft = newTimeLeft;
}

//________________________________ LimitedCollectorVolunteer_______________________________________

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders) :
 CollectorVolunteer(id, name, coolDown),
 maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const
{
    return ordersLeft > 0;
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return (hasOrdersLeft() && !isBusy());
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    int newTimeLeft = getCoolDown();
    setTimeLeft(newTimeLeft);
    ordersLeft = ordersLeft - 1;
}

int LimitedCollectorVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

string LimitedCollectorVolunteer::toString() const
{
    if (isBusy())
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy: True" +
               "\n" + "OrderId:" + std::to_string(getActiveOrderId()) + "\n" + "timeLeft:" + std::to_string(getTimeLeft()) +
               "\n" + "ordersLeft:" + std::to_string(getNumOrdersLeft());
    }
    else
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy: False" +
               "\n" + "OrderId: None" + "\n" + "timeLeft: None" +
               "\n" + "ordersLeft:" + std::to_string(getNumOrdersLeft());
    }
}



//_________________________________________DriverVolunteer________________________________________

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep) : 
Volunteer(id, name), 
maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(maxDistance){};

DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft()
{
    distanceLeft = distanceLeft - distancePerStep;
    if (distanceLeft < 0)
    {
        distanceLeft = 0;
    }
    return (distanceLeft <= 0);
}

// Decrease distanceLeft by distancePerStep,return true if distanceLeft<=0,false otherwise
bool DriverVolunteer::hasOrdersLeft() const
{
    return true;
}

bool DriverVolunteer::canTakeOrder(const Order &order) const
{ // Signal if the volunteer is not busy and the order is within the maxDistance
    return (!isBusy() && order.getDistance() <= getMaxDistance());
}

void DriverVolunteer::acceptOrder(const Order &order)
{ // Assign distanceLeft to order's distance
    activeOrderId = order.getId();
    distanceLeft = order.getDistance();
}

void DriverVolunteer::step()
{
    bool orderCompleted = decreaseDistanceLeft();
    if (orderCompleted)
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

string DriverVolunteer::toString() const
{
    if (isBusy())
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy: True" +
               "\n" + "OrderId:" + std::to_string(getActiveOrderId()) + "\n" + "timeLeft:" + std::to_string(getDistanceLeft()) +
               "\n" + "ordersLeft: No Limit";
    }

    else
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy: False" +
               "\n" + "OrderId: None" + "\n" + "timeLeft: None" +
               "\n" + "ordersLeft: No Limit";
    }
}


void DriverVolunteer::setDistanceLeft(int newDisLeft)
{
    distanceLeft = newDisLeft;
}

// __________________________________________LimitedDriverVolunteer_________________________________

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders) :
DriverVolunteer(id, name, maxDistance, distancePerStep),
maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    return (ordersLeft != 0);
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    return (!isBusy() && order.getDistance() <= getMaxDistance() && hasOrdersLeft());
}

// Signal if the volunteer is not busy, the order is within the maxDistance and have orders left
void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    int newDisLeft = getMaxOrders();
    setDistanceLeft(newDisLeft);
    ordersLeft = ordersLeft - 1;
}

// Reset distanceLeft to maxDistance and decrease ordersLeft ??
string LimitedDriverVolunteer::toString() const
{
    if (isBusy())
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy:" + std::to_string(isBusy()) +
               "\n" + "OrderId:" + std::to_string(getActiveOrderId()) + "\n" + "timeLeft:" + std::to_string(getDistanceLeft()) +
               "\n" + "ordersLeft:" + std::to_string(getNumOrdersLeft());
    }

    else
    {
        return "volunteerId:" + std::to_string(getId()) + "\n" + "isBusy:" + std::to_string(isBusy()) +
               "\n" + "OrderId: None" + "\n" + "timeLeft: None" +
               "\n" + "ordersLeft:" + std::to_string(getNumOrdersLeft());
    }
}

