// MATCHING_CONCESSION: register-pinning
#include "global.h"

// differs: tail copy of tag pointer (r2 vs r4) and constant register order

struct ArcTag {
    u8 pad0[4];
    u16 mode, field6;
    u32 elapsed;
    u16 fieldC, duration;
    u16 startX, startY, startZ;
};
struct ArcNpc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u8 pad10[8];
    u8 phase;
    u8 facing4, facing8, angle;
    u8 pad1C[4];
    struct ArcTag tag;
    u8 pad38[0x54 - 0x38];
    s16 targetX, targetY, targetZ;
    u8 pad5A[0x65 - 0x5A];
    u8 motionAngle;
    u8 pad66[2];
    s16 x, y, z;
};
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void TaggedSceneNpc_PlayTaggedAnimation(void *, s32, s32, s32, s32);
static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
void TaggedSceneNpc_UpdateArcMove(void *ctx, struct ArcNpc *npc)
{
    struct ArcTag *tag = &npc->tag;
    s16 *pos = &npc->x;
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        npc->angle = 0;
        npc->motionAngle = 0;
        npc->facing8 = 3;
        npc->facing4 = 1;
        TaggedSceneNpc_PlayTaggedAnimation(npc, 0x14, 1, 0, 4);
        tag->fieldC = 0x14;
        pos[0] = tag->startX;
        pos[1] = tag->startY;
        pos[2] = tag->startZ;
    }
    {
        u32 m = 0x400;
        u32 zero;
        npc->flags = m | npc->flags;
        zero = 0;
        switch (tag->mode) {
            case 0: {
                s32 dx, dz;
                struct ArcTag *t;
                s32 v;
                tag->field6 = zero;
                dx = npc->targetX - tag->startX;
                dz = npc->targetZ - tag->startZ;
                pos[0] = tag->startX + Div(tag->elapsed * dx, tag->duration);
                pos[2] = tag->startZ + Div(tag->elapsed * dz, tag->duration);
                v = 200 * gUnk_086149C4[Div(tag->elapsed << 7, tag->duration) & 0xff];
                t = tag;
                pos[1] = tag->startY + trunc12(v);
                if (tag->elapsed >= tag->duration) {
                    pos[1] = npc->targetY;
                    {
                        u32 one = 1;
                        u32 zero2 = 0;
                        asm("" : "+r"(zero2));
                        t->mode = one;
                        t->field6 = one;
                        t->elapsed = zero2;
                        npc->done = 1;
                    }
                }
                break;
            }
            case 1:
                tag->field6 = zero;
                break;
        }
    }
}
