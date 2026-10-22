#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

// MATCHING_CONCESSION: register-pinning
#define Crow_StopAtRaisedTerrain sub_080A06E8

#ifdef NONMATCHING
// fuzzy: 89.3%
// Remaining differences: register allocation and address lifetimes.
static inline s32 Height(s32 cell, struct ContactPosition *p)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
    case 1: height -= ((u8 *)p)[4]; break;
    case 2: height -= ((u8 *)p)[0]; break;
    }
    return height;
}
void Crow_StopAtRaisedTerrain(struct SceneActorRegistryResetView *input)
{
    register struct SceneActorRegistryResetView *actor asm("r5") = input;
    register u8 *context asm("r7") = actor->context;
    struct ContactPosition target;
    asm("" : "+r"(context));
    { u32 *flags = (u32 *)(context + 0x3d4); u32 mask = ~2; *flags &= mask; }
    if (actor->field2B5 == 0x25) {
        struct ContactPosition *p = &target;
        register struct ContactPosition *position asm("r2") = &actor->position;
        struct ContactPosition *offset = &actor->secondaryMotion;
        register struct ContactPosition *motion asm("r8");
        s32 cell, height;
        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        motion = (struct ContactPosition *)((u8 *)position + 8);
        position = motion;
        p->x = position->x + p->x;
        p->y = position->y + p->y;
        p->z = position->z + p->z;
        {
            s32 x = p->x >> 8;
            s32 z = p->z >> 8;
            asm("" : "=r"(motion) : "0"(position), "r"(x), "r"(z));
            if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                cell = 0;
            else cell = gUnk_030052F4->rows[z] + x;
        }
        height = Height(cell, &target);
        {
        register u32 off asm("r2") = 0x2be;
        u16 *ground = (u16 *)((u8 *)actor + off);
        off += 0x2c;
        if (*ground + *(s16 *)((u8 *)actor + off) < height) {
            actor->secondaryMotion.x = 0;
            actor->secondaryMotion.y = 0;
            actor->secondaryMotion.z = 0;
            *(u16 *)(context + 0x42c) = 0;
            *(u16 *)(context + 0x42e) = 0;
            *(u16 *)(context + 0x430) = 0;
            motion->x = 0;
            actor->motion.z = 0;
            actor->motion.y = 0;
            { u32 mask = 2; u32 *flags = (u32 *)(context + 0x3d4); *flags |= mask; }
        }
        }
        if (target.y < height) {
            actor->secondaryMotion.y = 0;
            actor->motion.y = 0;
            *(u16 *)(context + 0x42e) = 0;
            asm("" : : "r"(actor));
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080A06E8);
#endif
