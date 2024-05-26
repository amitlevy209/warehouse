#include <string>
#include <vector>
#include "WareHouse.h"
#include "Volunteer.h"
#include "Order.h"
#include "Customer.h"
#include "Action.h"
#include "Parser.h"

#include <iostream>
#include <ios>
#include <algorithm>
#include <sstream>

/////////////////////////////////////////////////////////////////////////RULE OF 5////////////////////////////////////////////////////////////////////

//____________constructor-reading congifille_______________//

WareHouse::WareHouse(const string &configFilePath) : defaultCust(new SoldierCustomer(-1, "defaultCust", -1, -1)),
                                                     defaultVol(new CollectorVolunteer(-1, "defaultVol", -1)),
                                                     defaultOrd(new Order(-1, -1, -1)),
                                                     orderCounter(0), isOpen(false),
                                                     actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(),
                                                     customerCounter(0), volunteerCounter(0)
{

    Parser::parseConfigFile(configFilePath, *this);
}

///________________copy constructor______________________//
WareHouse::WareHouse(const WareHouse &other) : defaultCust(other.defaultCust->clone()),
                                               defaultVol(other.defaultVol->clone()),
                                               defaultOrd(other.defaultOrd->clone()),
                                               orderCounter(other.orderCounter), isOpen(other.isOpen),
                                               actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(),
                                               customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter)
{

    copyVectors(other); // copy deep copy of all vectors
}

///_______________Destructor_____________________________//

WareHouse::~WareHouse()
{

    clearVectors();

    //  delete the defaults from the heap
    delete defaultCust;
    delete defaultVol;
    delete defaultOrd;
}

//______________Copy Assignment Operator___________________

WareHouse &WareHouse::operator=(const WareHouse &other)
{

    if (this != &other)
    { // check if it is the same element

        orderCounter = other.orderCounter;
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;

        // notice that although in our specific implementation the defults are identical and therefore do not have to be copied,
        //  but in order to maintain the correctness of the principles of the copy constructor as learned we chose to copy.
        delete defaultCust;
        delete defaultOrd;
        delete defaultVol;

        defaultCust = other.defaultCust->clone();
        defaultOrd = other.defaultOrd->clone();
        defaultVol = other.defaultVol->clone();

        clearVectors();     // clears all old iteam from this warehouse
        copyVectors(other); // copy deep copy of all vectors from other wareHouse
    }

    return *this;
}

//______________Move Constructor______________//

WareHouse::WareHouse(WareHouse &&other) : defaultCust(), defaultVol(), defaultOrd(), orderCounter(other.orderCounter), isOpen(other.isOpen),
                                          actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(),
                                          customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter)
{
    if (this != &other)
    { // check if this is not the same object

        // deal with "stealing" defaults-
        // notice that although in our specific implementation the defults are identical and stay the same,
        // we chose to "move" them in order to maintaim the principal and correctness of move constructor.
        defaultCust = other.defaultCust;
        defaultVol = other.defaultVol;
        defaultOrd = other.defaultOrd;

        // change other to null so they will not be deleted, becuse now they are in use by this
        other.defaultCust = nullptr;
        other.defaultVol = nullptr;
        other.defaultOrd = nullptr;

        // vectorstealer: steal all vectors by there pointer, and by all there element (deep "stealing"):

        for (auto *action : other.actionsLog)
        {
            actionsLog.push_back(action);
            action = nullptr;
        }
        actionsLog = other.actionsLog;

        for (auto *volunteer : other.volunteers)
        {
            volunteers.push_back(volunteer);
            volunteer = nullptr;
        }
        volunteers = other.volunteers;

        for (auto *order : other.pendingOrders)
        {
            pendingOrders.push_back(order);
            order = nullptr;
        }
        pendingOrders = other.pendingOrders;

        for (auto *order : other.inProcessOrders)
        {
            inProcessOrders.push_back(order);
            order = nullptr;
        }
        inProcessOrders = other.inProcessOrders;

        for (auto *order : other.completedOrders)
        {
            completedOrders.push_back(order);
            order = nullptr;
        }
        completedOrders = other.completedOrders;

        for (auto *customer : other.customers)
        {
            customers.push_back(customer);
            customer = nullptr;
        }
        customers = other.customers;
    }
}
//______________Move assignment operator______________//

WareHouse &WareHouse::operator=(WareHouse &&other)
{

    if (this != &other)
    { // check this is not the same object

        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        // deal with stealing the defaults
        delete defaultCust;
        delete defaultOrd;
        delete defaultVol;

        defaultCust = other.defaultCust;
        defaultOrd = other.defaultOrd;
        defaultVol = other.defaultVol;

        other.defaultCust = nullptr;
        other.defaultOrd = nullptr;
        other.defaultVol = nullptr;

        clearVectors();

        // VectorStealer: steal all vectors by there pointer, and by all there element (deep stealing):

        for (auto *action : other.actionsLog)
        {
            actionsLog.push_back(action);
            action = nullptr;
        }
        actionsLog = other.actionsLog;

        for (auto *volunteer : other.volunteers)
        {
            volunteers.push_back(volunteer);
            volunteer = nullptr;
        }
        volunteers = other.volunteers;

        for (auto *order : other.pendingOrders)
        {
            pendingOrders.push_back(order);
            order = nullptr;
        }
        pendingOrders = other.pendingOrders;

        for (auto *order : other.inProcessOrders)
        {
            inProcessOrders.push_back(order);
            order = nullptr;
        }
        inProcessOrders = other.inProcessOrders;

        for (auto *order : other.completedOrders)
        {
            completedOrders.push_back(order);
            order = nullptr;
        }
        completedOrders = other.completedOrders;

        for (auto *customer : other.customers)
        {
            customers.push_back(customer);
            customer = nullptr;
        }
        customers = other.customers;
    }

    return *this;
}

/////////////////////////////////////////////////////////WareHouse Methods/////////////////////////////////////////////////////////////////////////

void WareHouse::start()
{

    open();

    while (isOpen)
    {

        std::string userInput; // declare the total input excepted
        std::cout << "> Enter your command" << std::endl;
        std::getline(std::cin, userInput); // reads the input
        std::istringstream iss(userInput);

        std::string actionType; // referes to the first world in input which declare the wanted action
        iss >> actionType;

        // check the command in input:

        if (actionType == "step")
        {
            int numberOfSteps;
            iss >> numberOfSteps; // check the integer in input to get number of step
            SimulateStep *simulation = new SimulateStep(numberOfSteps);
            simulation->act(*this);
        }
        else if (actionType == "order")
        {
            int customerId;
            iss >> customerId;
            AddOrder *addingThisOrd = new AddOrder(customerId);
            addingThisOrd->act(*this);
        }
        else if (actionType == "customer")
        {
            std::string customerName, customerType;
            int customerDistance, maxOrders;
            iss >> customerName >> customerType >> customerDistance >> maxOrders;
            AddCustomer *addingThisCus = new AddCustomer(customerName, customerType, customerDistance, maxOrders);
            addingThisCus->act(*this);
        }
        else if (actionType == "orderStatus")
        {
            int orderId;
            iss >> orderId;
            PrintOrderStatus *printTHESEstatus = new PrintOrderStatus(orderId);
            printTHESEstatus->act(*this);
        }
        else if (actionType == "customerStatus")
        {
            int customerId;
            iss >> customerId;
            PrintCustomerStatus *printTHESEcustomers = new PrintCustomerStatus(customerId);
            printTHESEcustomers->act(*this);
        }
        else if (actionType == "volunteerStatus")
        {
            int VolunteerId;
            iss >> VolunteerId;
            PrintVolunteerStatus *printTHESEstatus = new PrintVolunteerStatus(VolunteerId);
            printTHESEstatus->act(*this);
        }
        else if (actionType == "log")
        {
            PrintActionsLog *printLog = new PrintActionsLog();
            printLog->act(*this);
        }

        else if (actionType == "close")
        {
            Close *closeWareHouse = new Close();
            closeWareHouse->act(*this);
        }
        else if (actionType == "backup")
        {
            BackupWareHouse *backupWareHouse = new BackupWareHouse();
            backupWareHouse->act(*this);
        }
        else if (actionType == "restore")
        {
            RestoreWareHouse *restoreWareHouse = new RestoreWareHouse();
            restoreWareHouse->act(*this);
        }
    }
}

void WareHouse::addOrder(Order *order)
{
    pendingOrders.push_back(order);
    orderCounter = orderCounter + 1;
}

void WareHouse::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}

Customer &WareHouse::getCustomer(int customerId) const
{
    for (Customer *customer : customers)
    {
        if (customer->getId() == customerId)
            return *customer;
    }

    return *defaultCust;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    for (Volunteer *volunteer : volunteers)
    {
        if (volunteer->getId() == volunteerId)
            return *volunteer;
    }

    return *defaultVol;
}

Order &WareHouse::getOrder(int orderId) const
{
    // check if the oreder is in the pending Orders
    for (Order *order : pendingOrders)
    {
        if (order->getId() == orderId)
            return *order;
    }
    // check if the oreder is in the process orders
    for (Order *order : inProcessOrders)
    {
        if (order->getId() == orderId)
            return *order;
    }
    // check if the oreder is in the completed orders
    for (Order *order : completedOrders)
    {
        if (order->getId() == orderId)
            return *order;
    }

    return *defaultOrd;
}

const vector<BaseAction *> &WareHouse::getActions() const
{
    return actionsLog;
}

void WareHouse::close()
{

    // prints all orders with their status:

    // step1- pass throw pendingOrders
    for (Order *order : pendingOrders)
    {
        std::cout << "OrderID: " << order->getId() << " , CustomerID: " << order->getCustomerId() << " , Status: " << order->OrderStatusToString() << std::endl;
    }
    // step2- pass throw process orders
    for (Order *order : inProcessOrders)
    {
        std::cout << "OrderID: " << order->getId() << " , CustomerID: " << order->getCustomerId() << " , Status: " << order->OrderStatusToString() << std::endl;
    }
    // step3- pass throw process orders
    for (Order *order : completedOrders)
    {
        std::cout << "OrderID: " << order->getId() << " , CustomerID: " << order->getCustomerId() << " , Status: " << order->OrderStatusToString() << std::endl;
    }

    isOpen = false;
}

void WareHouse::open()
{
    isOpen = true;

    std::cout << "Warehouse is open!" << std::endl;
}

/////////////////////////////////////////////////////Our added methods/////////////////////////////////////////////////////////////////////

//____________________________________getter for reching the vectors_________________________________________//
vector<Volunteer *> &WareHouse::getVolunteerVector()
{

    return volunteers;
}

vector<Order *> &WareHouse::getPendingOrders()
{

    return pendingOrders;
}

vector<Order *> &WareHouse::getinProcessOrders()
{

    return inProcessOrders;
}
vector<Order *> &WareHouse::getCompletedOrders()
{

    return completedOrders;
}

vector<Customer *> &WareHouse::getCustomerVector()
{

    return customers;
}


//__________________________________________________other added methods_______________________//

void WareHouse::PlusCustomerCounter()
{
    customerCounter = customerCounter + 1;
}

void WareHouse ::PlusVolunteerCounter()
{
    volunteerCounter = volunteerCounter + 1;
}

int WareHouse::getIdCustomer()
{
    return customerCounter;
}

//___________________________________________________for use in simulation_____________________________________________________//

Volunteer &WareHouse::findTheRightVolunteer(Order *order)
{
    OrderStatus orderStatus = order->getStatus();
    if (orderStatus == OrderStatus::PENDING)   // the order status is pending
    { 

        for (auto *volunteer : WareHouse::volunteers)
        {
            if (auto *collectorAsWanted = dynamic_cast<CollectorVolunteer *>(volunteer)) // if the volunteer is a collector
            {
               
                if (collectorAsWanted->canTakeOrder(*order))
                { 
                    return *volunteer;
                }
                
            }
        }
        return *defaultVol; // no volunteer avialable
    }

    if (orderStatus == OrderStatus::COLLECTING) // the order status is collecting
    { 
        for (auto *volunteer : WareHouse::volunteers)
        {

            if (auto *DriverAsWanted = dynamic_cast<DriverVolunteer *>(volunteer)) // if the Volunteer is a driver
            {

                if (DriverAsWanted->canTakeOrder(*order))
                { 
                    return *DriverAsWanted;
                }
            }
        }
        return *defaultVol; // no volunteer avialable
    }

    // if the orderStatus is not pending or collecting
    return *defaultVol;
}

void WareHouse::pendingToProcess(Order *order)
{
    inProcessOrders.push_back(order);
    pendingOrders.erase(find(pendingOrders.begin(), pendingOrders.end(), order));
}

void WareHouse::processToCompleted(Order *order)
{
    completedOrders.push_back(order);
    inProcessOrders.erase(find(inProcessOrders.begin(), inProcessOrders.end(), order));
}

void WareHouse::processToPending(Order *order)
{
    pendingOrders.push_back(order);
    inProcessOrders.erase(find(inProcessOrders.begin(), inProcessOrders.end(), order));
}

void WareHouse::deleteVolunteer(Volunteer *volunteer)
{
    auto it = find(volunteers.begin(), volunteers.end(), volunteer);
    if (it != volunteers.end())
    {
        volunteers.erase(it); // delete the volunteer from the vector

        delete volunteer; //delete from heap
       
    }
}

//_____________________________________________________________for destructor use______________________________________________________________//

//________________clearVectors________________//
void WareHouse::clearVectors()
{
    // delete all vectors with all there elements

    while (!actionsLog.empty())
    {
        auto *actionToDelete = actionsLog.back();
        if (actionToDelete != nullptr)
        {
            actionsLog.pop_back();
            delete actionToDelete;
        }
    }

    while (!volunteers.empty())
    {
        auto *volunteerToDelete = volunteers.back();
        if (volunteerToDelete != nullptr)
        {
            volunteers.pop_back();
            delete volunteerToDelete;
        }
    }

    while (!pendingOrders.empty())
    {
        auto *OrderToDelete = pendingOrders.back();
        if (OrderToDelete != nullptr)
        {
            pendingOrders.pop_back();
            delete OrderToDelete;
        }
    }

    while (!inProcessOrders.empty())
    {
        auto *OrderToDelete = inProcessOrders.back();
        if (OrderToDelete != nullptr)
        {
            inProcessOrders.pop_back();
            delete OrderToDelete;
        }
    }

    while (!completedOrders.empty())
    {
        auto *OrderToDelete = completedOrders.back();
        if (OrderToDelete != nullptr)
        {
            completedOrders.pop_back();
            delete OrderToDelete;
        }
    }

    while (!customers.empty())
    {
        auto *CustomerToDelete = customers.back();
        if (CustomerToDelete != nullptr)
        {
            customers.pop_back();
            delete CustomerToDelete;
        }
    }
}

//_____________________copyVectors_________________//

void WareHouse::copyVectors(const WareHouse &other)
{

    // Copy all vectors by all there element "deep copy":

    for (auto *action : other.actionsLog)
    {
        auto *copiedAction = action->clone();
        actionsLog.push_back(copiedAction);
    }

    for (auto *volunteer : other.volunteers)
    {
        auto *copiedVolunteer = volunteer->clone();
        volunteers.push_back(copiedVolunteer);
    }

    for (auto *order : other.pendingOrders)
    {
        auto *copiedOrder = order->clone();
        pendingOrders.push_back(copiedOrder);
    }

    for (auto *order : other.inProcessOrders)
    {
        auto *copiedOrder = order->clone();
        inProcessOrders.push_back(copiedOrder);
    }

    for (auto *order : other.completedOrders)
    {
        auto *copiedOrder = order->clone();
        completedOrders.push_back(copiedOrder);
    }

    for (auto *customer : other.customers)
    {
        auto *copiedCustomer = customer->clone();
        customers.push_back(copiedCustomer);
    }
}
