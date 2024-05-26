#include <iostream>
#include "Order.h"
#include "Volunteer.h"

// constructor
Order::Order(int id, int customerId, int distance) : id(id), customerId(customerId), distance(distance), 
status(OrderStatus::PENDING), collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {}

int Order::getId() const
{
    return id;
}

int Order::getCustomerId() const
{
    return customerId;
}

void Order::setStatus(OrderStatus status)
{
    this->status = status;
}

void Order::setCollectorId(int collectorId)
{
    this->collectorId = collectorId;
}

void Order::setDriverId(int driverId)
{
    this->driverId = driverId;
}

int Order::getCollectorId() const
{
    return collectorId;
}

int Order::getDriverId() const
{
    return driverId;
}

OrderStatus Order::getStatus() const
{
    return status;
}

int Order::getDistance() const
{
    return distance;
}

const string Order::toString() const
{

    string status = OrderStatusToString();
    // if there are no collector assigend yet
    if (collectorId == -1)  // NO_VOLUNTEER
    { 
        return "OrderId:" + std::to_string(id) + "\n" + "OrderStatus:" + status +
               "\n" + "CustomerID:" + std::to_string(customerId) + "\n" + "Collector: None" +
               "\n" + "Driver: None";
    }
    // if there are no driver assigend yet
    if (driverId == -1) // NO_VOLUNTEER
    { 
        return "OrderId:" + std::to_string(id) + "\n" + "OrderStatus:" + status +
               "\n" + "CustomerID:" + std::to_string(customerId) + "\n" + "Collector:" + std::to_string(collectorId) +
               "\n" + "Driver: None";
    }

    return "OrderId:" + std::to_string(id) + "\n" + "OrderStatus:" + status +
           "\n" + "CustomerID:" + std::to_string(customerId) + "\n" + "Collector:" + std::to_string(collectorId) +
           "\n" + "Driver:" + std::to_string(driverId);
}

void Order::acceptedOrder(Volunteer &volunteer)
{

    OrderStatus status = getStatus();

    if (status == OrderStatus::PENDING) // the volunteer is a collector
    { 
        setCollectorId(volunteer.getId());
        setStatus(OrderStatus::COLLECTING);
    }

    else  // the volunteer is a driver
    { 
        setDriverId(volunteer.getId());
        setStatus(OrderStatus::DELIVERING);
    }
}

string Order::OrderStatusToString() const
{
    OrderStatus status = getStatus();
    if (status == OrderStatus::PENDING)
    {
        return "PENDING";
    }

    else if (status == OrderStatus::COLLECTING)
    {
        return "COLLECTING";
    }

    else if (status == OrderStatus ::DELIVERING)
    {
        return "DELIVERING";
    }
    else if (status == OrderStatus::COMPLETED)
    {
        return "COMPLETED";
    }

    // Handle the case where 'status' doesn't match any of the above orderStatus
    return "Unknown";
}

Order *Order::clone()
{
    return new Order(*this);
}
