#ifndef GUARD_SCENE_ACTOR_DISPLACEMENT_H
#define GUARD_SCENE_ACTOR_DISPLACEMENT_H

#include "global.h"

/* Ease one quarter of the remaining displacement toward the desired coordinate.
 * The caller commits both axes after clamping their steps. */
static inline s32 SceneActorDisplacement_Quarter(s32 value)
{
    return value >= 0 ? value >> 2 : -(-value >> 2);
}
static inline s32 SceneActorDisplacement_ReadAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return SceneActorDisplacement_Quarter(difference);
}

#endif
