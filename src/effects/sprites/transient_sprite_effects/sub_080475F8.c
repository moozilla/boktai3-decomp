#include "effects/sprites/transient_sprite_effects.h"
/* TransientSpriteEffects_SpawnFromScript: Reads script keywords for effect kind options position velocity and damping then spawns a transient sprite effect */
s32 TransientSpriteEffects_Spawn(u32, u32, const struct ContactPosition *,
                                 const struct ContactPosition *, u32, u32);
s32 TransientSpriteEffects_SpawnFromScript(void)
{
    struct ContactPosition position, velocity;
    u32 kind = Script_GetKeywordValue(0x74, 0);
    u32 options = Script_GetKeywordValue(0x66, 0);
    u32 dragDelay, drag;
    if (Script_SeekToKeyword(0x70)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (Script_SeekToKeyword(0x73)) {
        velocity.x = Script_GetValue();
        velocity.y = Script_GetValue();
        velocity.z = Script_GetValue();
    } else {
        velocity.x = 0;
        velocity.y = 0;
        velocity.z = 0;
    }
    if (Script_SeekToKeyword(0x64)) {
        dragDelay = Script_GetValue();
        drag = Script_GetValue();
    } else {
        dragDelay = 24;
        drag = 256;
    }
    return TransientSpriteEffects_Spawn(kind, options, &position, &velocity, dragDelay, drag);
}
