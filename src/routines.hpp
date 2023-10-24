#pragma once
#include "structs.hpp"

// Master will use this to update and
// Slave will receive updates in this variable
extern CommunicationData communicationData;

void HandleRoutineSetup();

// Call in loop to do the routines
void HandleRoutines();

// Routine Handlers
// Update aControlData to make it do the routine for allow entry
void doAllowEntryRoutineUpdate();
// Update aControlData to make it do the routine for deny entry
void doDenyEntryRoutineUpdate();
// Maybe some Kind of breathing animation or something?
void doIdle();
// Cancel all routines
void cancelRoutines();

// Color Update
// Update the leds based on the current hat control data
void SendLEDStripUpdate();