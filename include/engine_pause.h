#ifndef GUARD_ENGINE_PAUSE_H
#define GUARD_ENGINE_PAUSE_H

#include "engine_globals.h"
#include "ram.h"

/* The engine and scene pause words read together: a set bit holds back one class of objects
 * (0x800 actors, colliders and script records; 0x400 effects). About eighty sites across the
 * game test it, every one loading the bit before either word. */
static inline u32 Engine_PauseBits(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

#endif
