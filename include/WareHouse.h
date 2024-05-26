#pragma once
#include <string>
#include <vector>
#include "Volunteer.h"
#include "Order.h"
#include "Customer.h"


class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.

class WareHouse {

    public:
        // rule of 5
        WareHouse(const string &configFilePath);
        WareHouse(const WareHouse& other); // copy constructor
        ~WareHouse(); //destructor
        WareHouse& operator=(const WareHouse& other); //Copy Assignment Operator
        WareHouse(WareHouse&& other); //Move Constructor
        WareHouse& operator=(WareHouse&& other); //Move assignment operator


        void start();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        const vector<BaseAction*> &getActions() const;
        void close();
        void open();

        //added methods:
        //getters for vectors
        vector<Volunteer*> &getVolunteerVector();
        vector<Order*> &getPendingOrders();
        vector<Order*> &getinProcessOrders();
        vector<Order*> &getCompletedOrders();
        vector<Customer*> &getCustomerVector();

        //other added
        void PlusCustomerCounter();
        void PlusVolunteerCounter(); 
        int getIdCustomer();
    

        //for use in simulation step
        Volunteer& findTheRightVolunteer(Order* order);
        void pendingToProcess (Order* order);
        void processToCompleted (Order* order);
        void processToPending (Order* order);
        void deleteVolunteer(Volunteer* volunteer);

        //for destructer use
        void clearVectors();
        void copyVectors(const WareHouse& other);    


        // public added fields
        SoldierCustomer* defaultCust;
        CollectorVolunteer* defaultVol;
        Order* defaultOrd;
        int orderCounter; // For assigning unique order IDs
   
    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        };



