#include "sprite_effect_controller.h"
#define SpriteEffectController_CreateFromScript sub_0806BA58
struct SpriteEffectController *sub_0806B5D4(const struct ContactPosition *,
                                            const struct ContactPosition *, s32, u32, u32, u32, u32,
                                            u32, u32, u32, u32, u32, s32, s32, s32, s32, s32, s32);
static inline s32 Read(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
void SpriteEffectController_CreateFromScript(void)
{
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } position, velocity;
    s32 priority, variant, animationFlags, lifetime;
    if (Script_SeekToKeyword(0x70)) {
        position.packed.x = Read();
        position.packed.y = Read();
        position.packed.z = Read();
    }
    if (Script_SeekToKeyword(0x76)) {
        velocity.packed.x = Read();
        velocity.packed.y = Read();
        velocity.packed.z = Read();
    }
    priority = Script_GetKeywordValue(0x50, 0);
    variant = Script_GetKeywordValue(0x64, 0);
    animationFlags = Script_GetKeywordValue(0x72, 0);
    lifetime = Script_GetKeywordValue(0x6c, 0);
    sub_0806B5D4(&position.position, &velocity.position, 0x1c1e, 0xd43f, 0, 0, 0x11, 1, priority,
                 variant, animationFlags, lifetime, 0, 0, 0, 0, -4, -4);
}
