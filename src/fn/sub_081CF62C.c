// MATCHING_CONCESSION: register-pinning
#include "scene_actor_terrain.h"
// confidence=low: animation 9-12 dash with decreasing speed and height above terrain.
struct NpcTag {
    u8 pad[10];
    s16 height;
};
struct Npc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u32 frame, timer;
    u8 phase, facing4, facing8, angle;
    u8 pad1C[4];
    struct NpcTag tag;
    u8 pad2C[0x68 - 0x2C];
    struct ContactPosition pos, vel;
    u8 pad78[0xD4 - 0x78];
    u8 display[0x208 - 0xD4];
    u32 speed;
    u8 pad20C[0x2E4 - 0x20C];
    u8 (*callback)(struct Npc *);
};
s32 DisplayAnimation_TestFinished(void *);
static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void polar(struct ContactPosition *v, u32 angle, s32 speed)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle] * speed);
}
void SceneNpc_UpdateDescendingDash(void *ctx, struct Npc *npc, s32 tick)
{
    struct NpcTag *tag = &npc->tag;
    if (npc->entered) {
        s32 zero = 0;
        npc->entered = zero;
        npc->done = zero;
        Actor_PlayAnimationEntry(npc, 9, 2, 0, 4);
        npc->phase = zero;
        npc->pos.y += 160;
        tag->height = 300;
    }
    {
        u32 mask = 0x409;
        npc->flags |= mask;
    }
    if (npc->callback(npc) != 1) {
        switch (npc->phase) {
            case 0:
                if (npc->frame > 3) {
                    Actor_PlayAnimationEntry(npc, 10, 2, 0, 4);
                    npc->phase++;
                    npc->frame = 0;
                }
                break;
            case 1: {
                struct ContactPosition *pos;
                polar(&npc->vel, npc->angle, npc->speed);
                pos = &npc->pos;
                npc->pos.y = tag->height +
                             SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
                npc->speed = (npc->speed * 3) >> 2;
                if (tick > 3) {
                    if (tag->height >= (tick << 3))
                        tag->height -= tick << 3;
                    else
                        tag->height = 0;
                }
                if (npc->speed <= 19 && tag->height == 0) {
                    Sound_PlaySong(680);
                    npc->phase++;
                    Actor_PlayAnimationEntry(npc, 11, 2, 0, 4);
                }
            } break;
            case 2:
                if (DisplayAnimation_TestFinished(npc->display)) {
                    Actor_PlayAnimationEntry(npc, 12, 0, 0, 4);
                    npc->phase++;
                }
                break;
            case 3:
                npc->done = 1;
                break;
        }
    }
}
