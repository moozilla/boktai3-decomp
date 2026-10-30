#include "scene_actor_chase.h"
#define Dog_RequestHeadingTurn sub_080DE42C
static inline s32 Absolute(s32 value)
{
    s32 result = value;
    if (result < 0)
        result = -result;
    return result;
}
static inline u16 Motion(struct SceneActorRegistryResetView *a, u32 mask)
{
    u32 bit = mask;
    u16 *field = &a->flags2D4;
    {
        u32 result = *field & bit;
        return result;
    }
}
static inline void SetTurn(struct SceneActorRegistryResetView *a, u32 value)
{
    u32 bits = 7;
    bits &= value;
    ((u8 *)a)[0x3CD] = bits;
}
u8 Dog_RequestHeadingTurn(struct SceneActorRegistryResetView *a, s32 heading)
{
    s32 difference = heading - a->body.direction;
    u16 motion;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    if (Absolute(difference) < a->tracking.fieldEA || a->field2B5 == 2)
        return 0;
    Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
    motion = Motion(a, 0x80);
    if (motion)
        Chase_SetBehavior(a, 13);
    else
        Chase_SetBehavior(a, 6);
    SetTurn(a, ((u16)heading + 0x70) >> 5);
    return 1;
}
