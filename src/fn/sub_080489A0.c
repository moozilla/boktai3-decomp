// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
static inline s32 scaleEmission(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void TetherActor_EmitDirectionalEffect(struct TetherActor *input)
{
    struct TetherActor *actor = input;
    u32 angle = ((actor->heading + 1) & 7) * 32;
    const s16 *sourceTable = gUnk_086149C4;
    const s16 *table;
    s32 rawX;
    s32 scaledX;
    s32 scaledZ;
    u32 highX;
    u16 y;
    struct ContactPackedVector position, velocity, size;
    rawX = sourceTable[(angle + 64) & 255] * 100;
    table = sourceTable;
    scaledX = scaleEmission(rawX);
    highX = (u32)((u16)actor->position.x + scaledX) << 16;
    asm("" : "+r"(highX), "+r"(actor));
    y = actor->position.y;
    position.x = highX >> 16;
    position.y = y;
    scaledZ = scaleEmission(table[angle & 255] * 100);
    position.z = actor->position.z + scaledZ;
    velocity.x = scaleEmission(table[(angle + 64) & 255] * 10);
    velocity.y = 0;
    velocity.z = scaleEmission(table[angle & 255] * 10);
    size.x = 32;
    size.y = 0;
    size.z = 32;
    SceneParticleBurst_CreateWhilePaused(&position, &velocity, &size, 1, 3, 4);
}
