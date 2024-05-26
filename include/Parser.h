#pragma once
#include "WareHouse.h"
#include <vector>
#include "Customer.h"
#include "Volunteer.h"
#include <string>

class Parser {
public:
    static void parseConfigFile(const std:: string& parsedConfi, WareHouse& warehouse);
};

