#pragma once
#include "structs.hpp"

// Call in loop to do the routines
void HandleRoutines();

// Routine Handlers
// Update the current state of device based on ControlData
void updateSelf(HatControlData &aControlData);
// Update aControlData to make it do the routine for allow entry
void doAllowEntryRoutineUpdate(HatControlData &aControlData);
// Update aControlData to make it do the routine for deny entry
void doDenyEntryRoutineUpdate(HatControlData &aControlData);
// Maybe some Kind of breathing animation or something?
void doIdle(HatControlData &aControlData);