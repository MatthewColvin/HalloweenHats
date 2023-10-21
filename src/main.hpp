#pragma once

#include "structs.hpp"
#include <Arduino.h>

// Master will use this to update and
// Slave will receive updates in this variable
extern CommunicationData communicationData;