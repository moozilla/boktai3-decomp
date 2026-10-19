#include "global.h"
#define ActorAnim_UpdateChildVisibility sub_081E9BA4

/* Tentative layout: four match-value words fit before the observed countdown.
 * The count bound and the meaning of the compared game-state value are unknown. */
struct ActorAnimChild {
    u32 flags;
    u8 pad0[0xac - 4];
    void *callback;
    u8 index, control;
    u8 pad1[6];
    s32 matchCount;
    u32 matchValues[4];
    u8 pad2[3];
    u8 countdown;
    u8 pad3[12];
};
struct ActorAnimController {
    u8 pad[0x1c];
    struct ActorAnimChild children[16];
};
extern u8 *gUnk_02000710;
s32 sub_08249240(void *, void *, void *);

static inline u8 matches(u32 a, u32 b)
{
    if (a == b)
        return 1;
    return 0;
}
void sub_081E9BA4(struct ActorAnimController *p)
{
    s32 i = 0, j;
    s32 offset;
    s32 *counts = (s32 *)(0xd4 + (u32)p);
    /* Preserve the ROM's observed read order across the inner scan. */
    volatile s32 *count = counts;
    u32 *baseFlags = (u32 *)p->children;
    volatile u32 *flags = baseFlags;
    offset = 0;
    do {
        s32 visible = 0;
        if (visible < *count) {
            u32 value = *(u32 *)(gUnk_02000710 + 0x5a0);
            s32 entryOffset = i * 0xdc;
            u8 *entryBase = (u8 *)p + 0xd8;
            j = *(s32 *)((u8 *)counts + offset);
            do {
                if (matches(value, *(u32 *)(entryBase + entryOffset)))
                    visible = 1;
                entryOffset += 4;
            } while (--j != 0);
        }
        if (visible)
            *flags &= ~1;
        else
            *flags |= 1;
        flags = (volatile u32 *)((u32)flags + 0xdc);
        count = (volatile s32 *)((u32)count + 0xdc);
        offset += 0xdc;
    } while (++i <= 15);
    i = 0;
    do {
        if ((*(u8 *)((u8 *)p + i * 0xdc + 0xcd) & 4) && !(*(u32 *)((u8 *)baseFlags + i * 0xdc) & 1)) {
            u8 *countdown = (u8 *)p + i * 0xdc + 0xeb;
            if (*countdown)
                --*countdown;
            {
                void **callback = (void **)(((u8 *)p + 0xc8) + i * 0xdc);
                void *child = (u8 *)p + (i * 0xdc + 0x1c);
                sub_08249240(p, child, *callback);
            }
        }
    } while (++i <= 15);
}
