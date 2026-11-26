#include "engine/shared/sprite_effect_controller.h"
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
static inline s32 Read(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
/* Requires p and v keyword triples. All five literal-key7314 GCX call sites
 * supply both (reviewed in SPRITE_EFFECT_STEERING_ACTORS.md); dynamic callers
 * must preserve that precondition. An absent keyword has no invented default. */
void SpriteEffectController_CreateFromScript(void)
{
    struct ContactPosition position, velocity;
    s32 priority, variant, animationFlags, lifetime;
    if (Script_SeekToKeyword('p')) {
        position.x = Read();
        position.y = Read();
        position.z = Read();
    }
    if (Script_SeekToKeyword('v')) {
        velocity.x = Read();
        velocity.y = Read();
        velocity.z = Read();
    }
    priority = Script_GetKeywordValue('P', 0);
    variant = Script_GetKeywordValue('d', 0);
    animationFlags = Script_GetKeywordValue('r', 0);
    lifetime = Script_GetKeywordValue('l', 0);
    SpriteEffectController_Create(&position, &velocity, 0x1c1e, 0xd43f, 0, 0, 0x11, 1, priority,
                                  variant, animationFlags, lifetime, 0, 0, 0, 0, -4, -4);
}
