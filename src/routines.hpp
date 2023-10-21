#pragma once
#include "structs.hpp"

// Call in loop to do the routines
void HandleRoutines();

// Routine Handlers
// Update aControlData to make it do the routine for allow entry
void doAllowEntryRoutineUpdate(HatControlData &aControlData);
// Update aControlData to make it do the routine for deny entry
void doDenyEntryRoutineUpdate(HatControlData &aControlData);
// Maybe some Kind of breathing animation or something?
void doIdle(HatControlData &aControlData);
// Cancel all routines
void cancelRoutines();

// Color Update
// Update the leds based on the current structures
void updateSelf(HatControlData &aControlData);