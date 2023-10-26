#pragma once
#include "structs.hpp"

// Master will use this to update and
// Slave will receive updates in this variable
extern CommunicationData communicationData;

void HandleRoutineSetup();

// Call in loop to do the routines
void HandleRoutines();

// Routine Handlers
// Update controlData to make it do the routine for allow entry
void doAllowEntryRoutineUpdate();
// Update controlData to make it do the routine for deny entry
void doDenyEntryRoutineUpdate();
// Update controlData to make it run the mario routine
void doMarioRoutineUpdate();
// Update controlData to do the jeopardy song
void doJeopardyRoutineUpdate();
// Maybe some Kind of breathing animation or something?
void doIdle();
// Cancel all routines
void cancelRoutines();

// Color Update
// Update the leds based on the current hat control data
void SendLEDStripUpdate();