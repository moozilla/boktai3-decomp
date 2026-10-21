#include "global.h"
struct VanargandFadePart {
    u8 unknown0[0x38];
    void *palette;
    u8 unknown3C[0x154];
    void *source;
    u8 unknown194[4];
    void *target;
    u8 unknown19C[0xc4];
};
struct VanargandFadePartState {
    u8 unknown0[0x160];
    u32 flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
};
void CpuSet(const void *, void *, u32);
void ActorStatus_Deactivate(void *, void *);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);
void Vanargand_UpdatePartFadeOut(u8 *boss, struct VanargandFadePartState *p, s32 frame)
{
    struct VanargandFadePart *q = (struct VanargandFadePart *)p;
    u32 k2 = 0x194;
    u32 twelve = 12;
    p->flags |= twelve;
    if (p->reset) {
        p->reset = 0;
        CpuSet(*(void **)((u8 *)p + 0x190), q->palette, 0x04000008);
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    {
        u32 offset = 0x164;
        u32 *statePtr;
        u32 value;
        statePtr = (u32 *)((u8 *)p - (-offset));
        value = *statePtr;
        switch (value) {
            case 0:
                Palette_InterpolateBanks(q->palette, *(void **)((u8 *)p + 0x190),
                                         *(void **)((u8 *)p + k2), frame, 6);
                if (frame > 63) {
                    u32 next = 1;
                    u32 off = 0x164;
                    u32 *state;
                    state = (u32 *)((u8 *)p + off);
                    *state = next;
                    {
                        u32 off2 = 0x168;
                        {
                            u32 *ptr = (u32 *)((u8 *)p + off2);
                            *ptr = 0;
                        }
                    }
                    {
                        u8 *ptr;
                        off += 11;
                        ptr = (u8 *)p - (-off);
                        *ptr = next;
                    }
                }
                break;
            case 1: {
                u32 off = 0x170;
                {
                    u8 *ptr = (u8 *)p + off;
                    *ptr = 1;
                }
                ActorStatus_Deactivate(boss, p);
            } break;
        }
    }
}
