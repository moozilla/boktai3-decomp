#include "tether_actor.h"
void TetherActor_EmitDirectionalEffect(struct TetherActor *);
u8 *sub_0821D64C(s32, s32);
void TetherActor_UpdateMovingEffects(struct TetherActor *actor)
{
    if (actor->soundCooldown)
        --actor->soundCooldown;
    if (!actor->moving)
        goto stopped;
    if (actor->effectsSuppressed)
        goto stopped;
    {
        if (actor->kind == 5) {
            if (!actor->effectFrame && !actor->soundCooldown) {
                Sound_PlaySong(0x26A);
                actor->soundCooldown = 100;
            }
        } else {
            if (!actor->effectFrame) {
                struct ContactPosition *position = &actor->position;
                s32 x = position->x >> 8;
                s32 z = position->z >> 8;
                s32 cell;
                u8 *properties;
                u32 surface;
                if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                    cell = 0;
                else
                    cell = gUnk_030052F4->rows[z] + x;
                properties = sub_0821D64C(cell, 1);
                if (properties)
                    properties += 4;
                else {
                    struct EngineMap *map = gUnk_030052F4;
                    u32 offset = cell * 4 + 12;
                    properties = map->cells + offset;
                }
                surface = *properties >> 4;
                if (surface - 1 <= 1)
                    Sound_PlaySong(0xD3);
                else if (actor->kind == 9)
                    Sound_PlaySong(0x4D8);
                else
                    Sound_PlaySong(0xD2);
            }
            if (!(actor->effectFrame & 7))
                TetherActor_EmitDirectionalEffect(actor);
        }
        ++actor->effectFrame;
    }
    return;
stopped:
    actor->effectFrame = 0;
}
