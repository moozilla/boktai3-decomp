#ifdef NONMATCHING
// fuzzy: 77.4%
/* Adjacent terrain heights, odd/even octants and displacement truncation are understood; allocation and tail layout still differ. */
#include "tether_actor.h"
extern u16 gUnk_02000550, gUnk_0200052C, gUnk_0200053C;
void sub_0821BC68(void *, void *);
u8 *sub_0821D64C(s32, s32);
static inline u32 cellByte(s32 cell)
{
    u8 *properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return *properties;
}
void TetherActor_AdjustSlopeMovement(struct TetherActor *actor)
{
    s32 octant;
    s32 neighbor;
    u32 height;
    s32 product;
    if (actor->kind != 9 || actor->state == 11 || gUnk_02000550 < gUnk_0200052C)
        return;
    if (!actor->movementProbe) {
        u32 offset = 0x120;
        void *probe = (u8 *)actor + offset;
        sub_0821BC68(probe, &actor->position);
        actor->movementProbe = probe;
    }
    {
        s32 direction = gUnk_0200053C + 112;
        direction &= 255;
        octant = direction >> 5;
    }
    neighbor = 0;
    if (1 & octant) {
        if (octant == 1) {
            neighbor = ((s16 *)gUnk_030052F4->cells)[2];
        } else if (octant == 3)
            neighbor = -1;
        else if (octant == 5)
            neighbor = -((s16 *)gUnk_030052F4->cells)[2];
        else if (octant == 7)
            neighbor = 1;
        {
            u32 value = cellByte(actor->movementProbeStorage.cell);
            height = 15;
            height &= value;
        }
        if (height < (cellByte(actor->movementProbeStorage.cell + neighbor) & 15))
            return;
        {
            s32 next = octant + 1;
            s32 angle = next << 5;
            s32 cosine = gUnk_086149C4[(angle + 64) & 255];
            s32 sine;
            const s16 *sample;
            s32 v;
            s32 delta;
            angle &= 255;
            sample = &gUnk_086149C4[angle];
            sine = *sample;
            v = gUnk_02000550 * cosine;
            if (v >= 0)
                delta = v >> 12;
            else
                delta = -((-v) >> 12);
            actor->movementDelta.x -= delta;
            product = gUnk_02000550 * sine;
        }
        goto adjustZ;
    } else {
        s32 x, z;
        if (octant == 0)
            neighbor = ((s16 *)gUnk_030052F4->cells)[2] + 1;
        else if (octant == 2)
            neighbor = ((s16 *)gUnk_030052F4->cells)[2] - 1;
        else if (octant == 4)
            neighbor = ~((s16 *)gUnk_030052F4->cells)[2];
        else if (octant == 6)
            neighbor = 1 - ((s16 *)gUnk_030052F4->cells)[2];
        {
            u32 value = cellByte(actor->movementProbeStorage.cell);
            height = 15;
            height &= value;
        }
        if (height < (cellByte(actor->movementProbeStorage.cell + neighbor) & 15))
            return;
        x = (u8)actor->position.x;
        z = (u8)actor->position.z;
        if (octant == 0) {
            if (x > z)
                neighbor = 1;
            else {
                neighbor = ((s16 *)gUnk_030052F4->cells)[2];
            }
        } else if (octant == 2) {
            if (x + z <= 256)
                neighbor = -1;
            else {
                neighbor = ((s16 *)gUnk_030052F4->cells)[2];
            }
        } else if (octant == 4) {
            if (x <= z)
                neighbor = -1;
            else
                neighbor = -((s16 *)gUnk_030052F4->cells)[2];
        } else if (octant == 6) {
            if (x + z > 256)
                neighbor = 1;
            else
                neighbor = -((s16 *)gUnk_030052F4->cells)[2];
        }
        {
            u32 value = cellByte(actor->movementProbeStorage.cell);
            height = 15;
            height &= value;
        }
        if (height < (cellByte(actor->movementProbeStorage.cell + neighbor) & 15))
            return;
        {
            s32 next = octant + 1;
            s32 angle = next << 5;
            s32 cosine = gUnk_086149C4[(angle + 64) & 255];
            s32 sine;
            const s16 *sample;
            s32 v;
            s32 delta;
            angle &= 255;
            sample = &gUnk_086149C4[angle];
            sine = *sample;
            v = gUnk_02000550 * cosine;
            if (v >= 0)
                delta = v >> 12;
            else
                delta = -((-v) >> 12);
            actor->movementDelta.x -= delta;
            product = gUnk_02000550 * sine;
        }
    }
adjustZ: {
    s32 delta;
    if (product >= 0)
        delta = product >> 12;
    else
        delta = -((-product) >> 12);
    actor->movementDelta.z -= delta;
}
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", TetherActor_AdjustSlopeMovement);
#endif
