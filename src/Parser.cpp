#include "Parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Customer.h"
#include "Volunteer.h"
#include "WareHouse.h"

void Parser::parseConfigFile(const std::string &parsedConfi, WareHouse &warehouse)
{

    std::ifstream configFile(parsedConfi);
    std::string command;
    int customerId = 0;
    int volunteerId = 0;

    while (getline(configFile, command))
    {
        if (command.empty() || command[0] == '#')
        { // Skip empty lines and notes
            continue;
        }

        std::istringstream iss(command);
        std::string actionType;
        iss >> actionType;

        if (actionType == "customer")
        {
            std::string name, customerType;
            int locationDistance, maxOrders;
            iss >> name >> customerType >> locationDistance >> maxOrders;

            if (customerType == "soldier")
            {
                SoldierCustomer* newCustomer = new SoldierCustomer(customerId, name, locationDistance, maxOrders);
                warehouse.getCustomerVector().push_back(newCustomer);
                customerId = customerId + 1;
                warehouse.PlusCustomerCounter();
            }
            else if (customerType == "civilian")
            {
                CivilianCustomer* newCustomer = new CivilianCustomer(customerId, name, locationDistance, maxOrders);
                warehouse.getCustomerVector().push_back(newCustomer);
                customerId = customerId + 1;
                warehouse.PlusCustomerCounter();
            }
        }

        else if (actionType == "volunteer")
        {

            std::string name, volunteerRole;
            int limitedParamter; // cooldown/ maxDistance
            int distancePerStep = 0;
            int maxOrders = -1;
            iss >> name >> volunteerRole >> limitedParamter;

            if (volunteerRole == "driver" || volunteerRole == "limited_driver")
            {
                iss >> distancePerStep;
            }
            if (volunteerRole == "limited_collector" || volunteerRole == "limited_driver")
            {
                iss >> maxOrders;
            }

            if (volunteerRole == "collector")
            {
                CollectorVolunteer *newVolunteer = new CollectorVolunteer(volunteerId, name, limitedParamter);
                warehouse.getVolunteerVector().push_back(newVolunteer);
                volunteerId = volunteerId + 1;
                warehouse.PlusVolunteerCounter();
            }
            else if (volunteerRole == "limited_collector")
            {
                LimitedCollectorVolunteer *newVolunteer = new LimitedCollectorVolunteer(volunteerId, name, limitedParamter, maxOrders);
                warehouse.getVolunteerVector().push_back(newVolunteer);
                volunteerId = volunteerId + 1;
                warehouse.PlusVolunteerCounter();
            }
            else if (volunteerRole == "driver")
            {
                DriverVolunteer *newVolunteer = new DriverVolunteer(volunteerId, name, limitedParamter, distancePerStep);
                warehouse.getVolunteerVector().push_back(newVolunteer);
                volunteerId = volunteerId + 1;
                warehouse.PlusVolunteerCounter();
            }
            else if (volunteerRole == "limited_driver")
            {
                LimitedDriverVolunteer *newVolunteer = new LimitedDriverVolunteer(volunteerId, name, limitedParamter, distancePerStep, maxOrders);
                warehouse.getVolunteerVector().push_back(newVolunteer);
                volunteerId = volunteerId + 1;
                warehouse.PlusVolunteerCounter();
            }
         
        }
    }
}
