#ifndef GUARD_ENGINE_GLOBALS_H
#define GUARD_ENGINE_GLOBALS_H

#include "global.h"

/* Engine-wide IWRAM objects (rows of symbols/ram.csv with header include/engine_globals.h).
 * Scalars and pointers to incomplete structs only, so any draft can include this. */
struct TaskList {
    struct Task *head;
    u32 pauseMask; /* list is skipped while (gEnginePauseFlags & pauseMask) */
};

/* 0x03005240 */
extern u16 gNextTaskId; /* next Task id (unique, never 0) */
/* 0x03005280 */
extern struct TaskList gTaskLists[14]; /* task lists by priority */
/* 0x030052F4 */
extern struct EngineMap *gTerrainMap; /* current terrain map */
/* 0x030052F8 */
extern u32 gTerrainWidth; /* terrain map width (cells) */
/* 0x030052FC */
extern u32 gTerrainHeight; /* terrain map height (cells) */
/* 0x03005308 */
extern u32 gRandomIndex; /* index (0..1023) into the random table */
/* 0x030053F4 */
extern u32 gEngineFlags; /* more engine flags (bit 10 mutes Sound_PlaySongChecked; guess) */
/* 0x030039F0 */
extern u16 gScriptEntryProcedure; /* script entry procedure selected by Script_SetEntryProcedure */

#endif
