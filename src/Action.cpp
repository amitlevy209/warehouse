#include <string>
#include <vector>
#include "Action.h"
#include "WareHouse.h"
#include "Volunteer.h"
#include "Order.h"
#include <iostream>
#include <sstream>
using std::string;
using std::vector;

extern WareHouse *backup;

// _________________________________ public BaseAction_________________________________

BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::COMPLETED) {}

ActionStatus BaseAction::getStatus() const
{
    return status;
}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED; // update status to be completed
}

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR; // update status to be error
    errorMsg = errorMsg;
    std::cout << "Error:" << errorMsg << std::endl;
}

string BaseAction::getErrorMsg() const
{
    return errorMsg;
}

string BaseAction::StatusToString() const
{
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "COMPLETED";
    }
    else
        return "ERROR";
}

string BaseAction::OrderStatusToString(const OrderStatus status)
{
    if (status == OrderStatus::PENDING)
        return "PENDING";

    else if (status == OrderStatus::COLLECTING)
        return "COLLECTING";

    else if (status == OrderStatus ::DELIVERING)
        return "DELIVERING";

    else if (status == OrderStatus::COMPLETED)
        return "COMPLETED";

    // Handle the case where 'status' doesn't match any of the above status
    return "Unknown";
}
// _________________________ SimulateStep  : public BaseAction_________________________________
SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(WareHouse &wareHouse)
{
    for (int steps = 0; steps < numOfSteps; steps = steps + 1)
    {

        // step 1: go through all orders and hand them over to the next operation
        for (std::size_t ordersCheck = 0; ordersCheck < wareHouse.getPendingOrders().size();)
        { 

            // check the order and it requirment, and finds the right volunteer to take it
            Order *orderToDeal = wareHouse.getPendingOrders()[ordersCheck];
            Volunteer &choosenVolunteer = wareHouse.findTheRightVolunteer(orderToDeal);

            // only if found a volunteer to take the order as needed
            if (choosenVolunteer.getId() != NO_VOLUNTEER)
            {
                choosenVolunteer.acceptOrder(*orderToDeal);   // accept the order by the volunteer
                orderToDeal->acceptedOrder(choosenVolunteer); // update the fields of the order
                wareHouse.pendingToProcess(orderToDeal);      // update the warehouse vectors
            }
            else
            {
                ordersCheck = ordersCheck + 1; // couldnt find volunteer to that order, chcek next one.
            }
        }

        // step 2: decrese the time left

        vector<Volunteer *> &forLoopingV = wareHouse.getVolunteerVector();
        for (auto *thisVolunteer : forLoopingV) // check auto beacause need "step" will be sent to volunteer type correctly
        {
            if (thisVolunteer->isBusy())
                thisVolunteer->step();
        }

        // step 3: go throw all voluenteers and update there orders

        vector<Volunteer *> &forLoopingV2 = wareHouse.getVolunteerVector(); // //check auto beacause need "step" will be sent to volunteer type correctly
        for (auto *thisVolunteer : forLoopingV2)
        {
            if (thisVolunteer->getCompletedOrderId() != NO_ORDER)
            {

                // check the order that the volunteer completed and get acsess to it
                int orderWHATvol = thisVolunteer->getCompletedOrderId();
                Order *order = &wareHouse.getOrder(orderWHATvol);

                if (order->getStatus() == OrderStatus::COLLECTING)  // means the volunteer is a collector
                {                                     
                    wareHouse.processToPending(order); // therefore move the order from inProcess vector to pending vector in warehouse
                }
                else // the orderStatus was delivering means the volunteer is a driver
                {
                    wareHouse.processToCompleted(order); // therefore move the order from inProcess vector to completed vector in warehouse
                    order->setStatus(OrderStatus::COMPLETED);
                }

                thisVolunteer->updateCompleted();
            }
        }

        // step 4: delete volenteers that reach there maxOrders

        for (std::size_t volunteerCheck = 0; volunteerCheck < wareHouse.getVolunteerVector().size();)
        {
            Volunteer *VolunteerToDeal = wareHouse.getVolunteerVector()[volunteerCheck];
            if (!VolunteerToDeal->hasOrdersLeft() && !VolunteerToDeal->isBusy()) // needs to be deleted
            {
                wareHouse.deleteVolunteer(VolunteerToDeal);
            }
            else
            {
                volunteerCheck = volunteerCheck + 1;
            }
        }
    }

    complete();
    wareHouse.addAction(this);
}

string SimulateStep::toString() const
{
    string status = StatusToString();
    string ans = "simulateStep " + std::to_string(numOfSteps) + " " + status;
    return ans;
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}

// _________________________ AddOrder  : public BaseAction_________________________________

AddOrder::AddOrder(int id) : customerId(id) {}

void AddOrder::act(WareHouse &wareHouse)
{

    // bring the customer from the customers vector
    Customer &CustomerCheck = wareHouse.getCustomer(customerId);

    // check if the customer exist and if he can make another order
    if (CustomerCheck.getId() != wareHouse.defaultCust->getId() && CustomerCheck.canMakeOrder())
    {
        int distance = CustomerCheck.getCustomerDistance();
        int orderId = wareHouse.orderCounter;
        Order *newOrder = new Order(orderId, customerId, distance); // create a new order
        CustomerCheck.addOrder(orderId);                            // add the order to the customer orders
        wareHouse.addOrder(newOrder);                               // add to the warehouse orders
        complete();                                                 // change the action ststus to complete
    }
    else
    {
        error("Cannot place this order"); // return an eror if action failes
    }
    wareHouse.addAction(this);
}

string AddOrder::toString() const
{
    string status = StatusToString();
    string ans = "order " + std::to_string(customerId) + " " + status;
    return ans;
}

AddOrder *AddOrder::clone() const
{
    return new AddOrder(*this);
};
// _________________________ AddCustomer  : public BaseAction_________________________________

AddCustomer ::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders) : 
customerName(customerName), customerType(stringToEnum(customerType)),
distance(distance), maxOrders(maxOrders), CustomerTypeString(customerType){};

void AddCustomer::act(WareHouse &wareHouse)
{

    int id = wareHouse.getIdCustomer();

    if (CustomerTypeString == "Soldier")
    {
        SoldierCustomer *newCustomer = new SoldierCustomer(id, customerName, distance, maxOrders);
        wareHouse.getCustomerVector().push_back(newCustomer); // adding the new customer to the new vector.
        wareHouse.PlusCustomerCounter();
    }
    else
    {
        CivilianCustomer *newCustomer = new CivilianCustomer(id, customerName, distance, maxOrders);
        wareHouse.getCustomerVector().push_back(newCustomer); // adding the new customer to the new vector.
        wareHouse.PlusCustomerCounter();
    }

    complete();

    wareHouse.addAction(this);
};

AddCustomer *AddCustomer::clone() const
{
    return new AddCustomer(*this);
}

string AddCustomer::toString() const
{
    string status = StatusToString();
    string ans = "Customer " + customerName + " " + CustomerTypeString +
                 +" " + std::to_string(distance) + " " + std::to_string(maxOrders) + " " + status;
    return ans;
}

const CustomerType AddCustomer::stringToEnum(const string &customerType) const
{
    if (customerType == "Soldier")
        return CustomerType::Soldier;
    else
        return CustomerType::Civilian;
}

// _________________________ PrintOrderStatus : public BaseAction_________________________________

PrintOrderStatus ::PrintOrderStatus(int id) : orderId(id){};

void PrintOrderStatus::act(WareHouse &wareHouse)
{
    Order &thisOrder = wareHouse.getOrder(orderId);

    if (thisOrder.getId() != wareHouse.defaultOrd->getId())
    {
        string ans = thisOrder.toString();
        std::cout << ans << std::endl;

        complete();
    }

    else
    {
        error("Order doesn't exist");
    }

    wareHouse.addAction(this);
}

PrintOrderStatus *PrintOrderStatus::clone() const
{
    return new PrintOrderStatus(*this);
}

string PrintOrderStatus::toString() const
{
    string status = StatusToString();
    string ans = "orderStatus " + std::to_string(orderId) + " " + status;
    return ans;
}
// ____________________________PrintCustomerStatus: public BaseAction_________________________________

PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId){};

void PrintCustomerStatus::act(WareHouse &wareHouse)
{

    Customer &thisCustomer = wareHouse.getCustomer(customerId);

    if (thisCustomer.getId() != wareHouse.defaultCust->getId())
    {
        // print customer id
        std::cout << "CustomerId:" << customerId << std::endl;

        // print the customer orders status
        for (int orderId : thisCustomer.getOrdersIds())
        {
            std::cout << "OrderId:" << orderId << std::endl;
            OrderStatus thisOrderStatus = wareHouse.getOrder(orderId).getStatus();
            std::cout << "OrderStatus:" << OrderStatusToString(thisOrderStatus) << std::endl;
        }

        // print the num of orders left
        std::cout << "numOrdersLeft:" << std::to_string(thisCustomer.getMaxOrders() - thisCustomer.getNumOrders()) << std::endl;

        complete();
    }
    else
    {
        error("Customer doesn't exist");
    }

    wareHouse.addAction(this);
}

PrintCustomerStatus *PrintCustomerStatus::clone() const
{
    return new PrintCustomerStatus(*this);
}

string PrintCustomerStatus::toString() const
{
    string status = StatusToString();
    string ans = "CustomerStatus " + std::to_string(customerId) + " " + status;
    return ans;
}

//_____________________________PrintVolunteerStatus : public BaseAction_________________________________

PrintVolunteerStatus::PrintVolunteerStatus(int id) : volunteerId(id){};

void PrintVolunteerStatus::act(WareHouse &wareHouse)
{
    Volunteer &checkVolunteer = wareHouse.getVolunteer(volunteerId);

    if (checkVolunteer.getId() != wareHouse.defaultVol->getId())
    {
        string ans = checkVolunteer.toString();
        std::cout << ans << std::endl;

        complete();
    }
    else
    {
        error("Volunteer doesn't exist");
    }

    wareHouse.addAction(this);
}
PrintVolunteerStatus *PrintVolunteerStatus::clone() const
{
    return new PrintVolunteerStatus(*this);
}

string PrintVolunteerStatus::toString() const
{
    string status = StatusToString();
    string ans = "VolunteerStatus " + std::to_string(volunteerId) + " " + status;
    return ans;
}
//_____________________________PrintActionsLog : public BaseAction_________________________________

PrintActionsLog ::PrintActionsLog(){};

void PrintActionsLog ::act(WareHouse &wareHouse)
{

    for (auto &action : wareHouse.getActions()) // use auto because baseaction is abstract
    { 
        std::cout << action->toString() << std::endl;
    }

    complete();
    wareHouse.addAction(this);
    toString(); // needs to print also this action
}

PrintActionsLog *PrintActionsLog ::clone() const
{
    return new PrintActionsLog(*this);
}

string PrintActionsLog ::toString() const
{
    string status = StatusToString();
    string ans = "log " + status;
    return ans;
};
//_______________________________Close : public BaseAction_________________________________

Close::Close(){};

void Close::act(WareHouse &wareHouse)
{
    wareHouse.close(); // print, change isopen, delete (release memory)
    complete();
    wareHouse.addAction(this);
}

Close *Close::clone() const
{
    return new Close(*this);
}

string Close::toString() const
{
    string status = StatusToString();
    string ans = "close" + status;
    return ans;
}

//______________________________BackupWareHouse : public BaseAction_______________________________________

BackupWareHouse ::BackupWareHouse(){};
void BackupWareHouse::act(WareHouse &wareHouse)
{
    if (backup == nullptr){
        backup = new WareHouse(wareHouse); // call wareHouse copy constructor
        }
    else
    {
        delete backup; 
        backup = new WareHouse(wareHouse); 
    }

    wareHouse.addAction(this);
    complete();
}

BackupWareHouse *BackupWareHouse::clone() const
{
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const
{
    string status = StatusToString();
    string ans = "backup " + status;
    return ans;
}

;
//______________________________RestoreWareHouse : public BaseAction_______________________________________

RestoreWareHouse::RestoreWareHouse(){};

void RestoreWareHouse::act(WareHouse &wareHouse)
{
    if (backup != nullptr)
    {
        complete();
        wareHouse = *backup;       // use assignment operator
        wareHouse.addAction(this); // after back up because we want restore action will be include in actionLog
    }
    else
    {
        error("No backup available");
        wareHouse.addAction(this);
    }
}

RestoreWareHouse *RestoreWareHouse::clone() const
{
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const
{
    string status = StatusToString();
    string ans = "restore " + status;
    return ans;
}

;