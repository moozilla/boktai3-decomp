#ifndef GUARD_SCENE_PAUSE_H
#define GUARD_SCENE_PAUSE_H

#include "engine/engine_globals.h"
#include "engine/ram.h"

/* The engine and scene pause words are tested together (bit 10 holds every
 * effect spawn); some eighty sites do this, several through per-header copies
 * (Engine_PauseBits, RegistryActor_MessagePauseBits). */
static inline u32 Scene_TestPauseFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

#endif
