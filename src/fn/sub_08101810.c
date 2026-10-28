#define gNamedObjectRandomIndex gRandomIndex
#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
struct NamedObjectJitterState {
    u8 unknown00[0x1C];
    struct SceneVec position;
    u8 unknown24[0xB8 - 0x24];
    s16 flags;
    u8 unknownBA[8];
    s16 verticalImpulse;
};
s32 Div(s32, s32);
s32 Mod(s32, s32);
static inline u8 HasFlag(u32 mask, s16 *flags)
{
    if ((u16)*flags & mask)
        return TRUE;
    return FALSE;
}
static inline u16 RandomValue(void)
{
    return gRandomTable[gRandomIndex = (gRandomIndex + 1) & 0x3FF];
}
static inline void ClearFlag(s32 mask, s16 *flags)
{
    *flags &= ~mask;
}
void NamedObject_ApplyVerticalImpulseAndJitter(struct NamedObjectJitterState *object)
{
    if (object->verticalImpulse < 0) {
        object->position.y += object->verticalImpulse;
        object->verticalImpulse = Div(object->verticalImpulse, 2);
    }
    if (HasFlag(0x80, &object->flags))
        goto clear;
    if (HasFlag(0x20, &object->flags)) {
        object->position.x += Mod(RandomValue(), 12) - 6;
        {
            u32 next = (gRandomIndex + 1) & 0x3FF;
            u32 value = gRandomTable[next];
            value &= 3;
            object->position.y += value;
            gRandomIndex = (next + 1) & 0x3FF;
            object->position.z += Mod(gRandomTable[gRandomIndex], 12) - 6;
        }
    clear:
        ClearFlag(0x20, &object->flags);
    }
}
