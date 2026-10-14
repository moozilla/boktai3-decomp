#include "transient_sprite_effects.h"
#define TransientSpriteEffects_SpawnFromScript sub_080475F8
s32 TransientSpriteEffects_Spawn(u32, u32, const struct ContactPosition *,
                                 const struct ContactPosition *, u32, u32);
s32 TransientSpriteEffects_SpawnFromScript(void)
{
    union {
        struct ContactPackedVector bits;
        struct ContactPosition vector;
    } position, velocity;
    u32 kind = Script_GetKeywordValue(0x74, 0);
    u32 options = Script_GetKeywordValue(0x66, 0);
    u32 dragDelay, drag;
    if (Script_SeekToKeyword(0x70)) {
        position.bits.x = Script_GetValue();
        position.bits.y = Script_GetValue();
        position.bits.z = Script_GetValue();
    } else {
        position.bits.x = 0;
        position.bits.y = 0;
        position.bits.z = 0;
    }
    if (Script_SeekToKeyword(0x73)) {
        velocity.bits.x = Script_GetValue();
        velocity.bits.y = Script_GetValue();
        velocity.bits.z = Script_GetValue();
    } else {
        velocity.bits.x = 0;
        velocity.bits.y = 0;
        velocity.bits.z = 0;
    }
    if (Script_SeekToKeyword(0x64)) {
        dragDelay = Script_GetValue();
        drag = Script_GetValue();
    } else {
        dragDelay = 24;
        drag = 256;
    }
    return TransientSpriteEffects_Spawn(kind, options, &position.vector, &velocity.vector,
                                        dragDelay, drag);
}
