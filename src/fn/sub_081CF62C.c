#include "global.h"
#include "engine.h"
#include "scene_vec.h"
#define SceneNpc_UpdateDescendingArrival sub_081CF62C
// confidence=low: the state settles onto terrain before finishing an arrival animation.
struct NpcTag {
    u8 pad[0xA];
    s16 height;
};
struct Npc {
    u8 pad0[7];
    u8 done, pad8, enter;
    u8 padA[4];
    u16 flags;
    u32 frame, timer;
    u8 phase, facing4, facing8, angle;
    u8 pad1C[4];
    struct NpcTag tag;
    u8 pad2C[0x68 - 0x2C];
    struct SceneVec pos, velocity;
    u8 pad78[0x208 - 0x78];
    u32 speed;
    u8 pad20C[0x2E4 - 0x20C];
    s32 (*callback)(struct Npc *);
};
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 DisplayAnimation_TestFinished(void *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 fixed(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void direction(struct SceneVec *out, u32 angle, s32 speed)
{
    out->x = fixed(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = fixed(speed * gUnk_086149C4[angle]);
}
static inline s32 terrainHeight(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    s32 index;
    u8 *cell;
    u32 kind;
    s32 height;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gUnk_030052F4->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= ((u8 *)pos)[4];
            break;
        case 2:
            height -= ((u8 *)pos)[0];
            break;
    }
    return height;
}
void SceneNpc_UpdateDescendingArrival(void *context, struct Npc *p, s32 elapsed)
{
    struct NpcTag *tag = &p->tag;
    if (p->enter) {
        s32 zero = 0;
        p->enter = zero;
        p->done = zero;
        Actor_PlayAnimationEntry(p, 9, 2, 0, 4);
        p->phase = zero;
        p->pos.y += 160;
        tag->height = 300;
    }
    {
        u32 mask = 0x409;
        p->flags |= mask;
    }
    if ((u8)p->callback(p) == 1)
        return;
    switch (p->phase) {
        case 0:
            if (p->frame > 3) {
                Actor_PlayAnimationEntry(p, 10, 2, 0, 4);
                p->phase++;
                p->frame = 0;
            }
            break;
        case 1:
            direction(&p->velocity, p->angle, p->speed);
            p->pos.y = tag->height + terrainHeight(&p->pos);
            p->speed = p->speed * 3 >> 2;
            if (elapsed > 3) {
                if (tag->height >= elapsed * 8)
                    tag->height -= elapsed * 8;
                else
                    tag->height = 0;
            }
            if (p->speed <= 19 && tag->height == 0) {
                Sound_PlaySong(680);
                p->phase++;
                Actor_PlayAnimationEntry(p, 11, 2, 0, 4);
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished((u8 *)p + 0xD4)) {
                Actor_PlayAnimationEntry(p, 12, 0, 0, 4);
                p->phase++;
            }
            break;
        case 3:
            p->done = 1;
            break;
    }
}
