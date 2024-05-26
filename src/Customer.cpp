#include "Customer.h"
#include <string>
#include <vector>
using std::string;
using std::vector;


Customer::Customer(int id, const string &name, int locationDistance, int maxOrders) :
 id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders), ordersId() {}

const string &Customer::getName() const
{
    return name;
}

int Customer::getId() const
{
    return id;
}

int Customer::getCustomerDistance() const
{
    return locationDistance;
}

// Returns maxOrders
int Customer::getMaxOrders() const
{
    return maxOrders;
}

// Returns num of orders the customer has made so far
int Customer::getNumOrders() const
{
    return ordersId.size();
}

// Returns true if the customer didn't reach max orders
bool Customer::canMakeOrder() const
{
    int numOfOrders = static_cast<int>(ordersId.size()); // long to int
    return numOfOrders < maxOrders;
}

const vector<int> &Customer::getOrdersIds() const
{
    return ordersId;
}
// return OrderId if order was added successfully, -1 otherwise
int Customer::addOrder(int _orderId)
{
    if (canMakeOrder())
    {
        ordersId.push_back(_orderId);

        return _orderId;
    }

    else
    {
        return -1;
    }
}

//________________________________soldierCustomer_________________________________
SoldierCustomer::SoldierCustomer(int _id, string _name, int _locationDistance, int _maxOrders) : 
Customer(_id, _name, _locationDistance, _maxOrders) {}

SoldierCustomer *SoldierCustomer::clone() const
{
    return new SoldierCustomer(*this);
}

//________________________________CivilianCustomer_________________________________
CivilianCustomer::CivilianCustomer(int _id, string _name, int _locationDistance, int _maxOrders) : 
Customer(_id, _name, _locationDistance, _maxOrders) {}

CivilianCustomer *CivilianCustomer::clone() const
{
    return new CivilianCustomer(*this);
}
