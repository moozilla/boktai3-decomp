#include "tether_actor.h"
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
static inline s32 scaleEmission(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void TetherActor_EmitRandomEffect(struct TetherActor *actor)
{
    u32 angle;
    const u8 *sample;
    u32 mask;
    struct ContactPackedVector velocity, position, size;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    sample = (const u8 *)RANDOM_TABLE_ADDR + gUnk_03005308 * 2;
    mask = 255;
    angle = *sample;
    velocity.x = scaleEmission(gUnk_086149C4[(angle + 64) & mask] * 32);
    velocity.y = 0;
    velocity.z = scaleEmission(gUnk_086149C4[angle] * 32);
    position.x = actor->position.x + (u16)velocity.x * 4;
    position.y = actor->position.y;
    position.z = actor->position.z + (u16)velocity.z * 4;
    size.x = 0;
    size.y = 0;
    size.z = 0;
    SceneParticleBurst_CreateWhilePaused(&position, &velocity, &size, 1, 3, 4);
    if (angle & 1)
        Sound_PlaySong(0xD3);
}
