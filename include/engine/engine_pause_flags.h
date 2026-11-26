#ifndef GUARD_ENGINE_PAUSE_FLAGS_H
#define GUARD_ENGINE_PAUSE_FLAGS_H

#include "engine/video/render.h"

/* Pause and resume the task lists whose pauseMask shares one of these bits (gTaskLists). */
static inline void Engine_SetPauseFlags(u32 flags)
{
    gEnginePauseFlags |= flags;
}

static inline void Engine_ClearPauseFlags(u32 flags)
{
    gEnginePauseFlags &= ~flags;
}

#endif
