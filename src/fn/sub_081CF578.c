#include "global.h"

extern const s16 gUnk_086149C4[];
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
struct Vec {
    s16 x, y, z, pad;
};
struct DashNpc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u32 frame, timer;
    u8 phase, facing4, facing8, angle;
    u8 pad1C[0x70 - 0x1C];
    struct Vec vel;
    u8 pad78[0x208 - 0x78];
    u32 speed;
    u8 pad20C[0x2E4 - 0x20C];
    u8 (*callback)(struct DashNpc *);
};
void SceneNpc_UpdateDashSlide(void *ctx, struct DashNpc *npc)
{
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 9, 2, 0, 4);
    }
    if (npc->callback(npc) != 1) {
        u32 speed = npc->speed;
        if (speed == 0) {
            npc->done = 1;
            return;
        }
        {
            struct Vec *vel = &npc->vel;
            u8 angle = npc->angle;
            u32 spd = speed;
            vel->x = trunc12(spd * gUnk_086149C4[(angle + 0x40) & 0xff]);
            vel->y = 0;
            vel->z = trunc12(spd * gUnk_086149C4[angle]);
        }
        npc->speed = npc->speed * 3 / 4;
    }
}
