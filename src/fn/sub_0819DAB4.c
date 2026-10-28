#include "global.h"
#include "ram.h"

// confidence=low: initial attack sequence before arena departure.

struct Action {
    u8 state, pad[3];
    u32 frame;
};
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void ActionState_AdvanceState(void *);
// confidence=low: selects attacks until the arena departure sequence completes.
static inline u8 Start(u8 *p)
{
    u8 *flag = p + 0x9a;
    s32 zero = 0;
    *flag = 1;
    if (p[0xb0]) {
        p[0xb0] = zero;
        p[0xaf] = zero;
        return 1;
    }
    return 0;
}
static inline void Request(u8 *p, u32 kind)
{
    p[0xaa] = kind;
}
static inline u32 TestFlags(u8 *p, u32 mask)
{
    return *(u32 *)(p + 0x9c) & mask;
}
static inline u32 PlayerFlags(u8 *p, u32 mask)
{
    return *(u32 *)(p + 0x20) & mask;
}
void Hresvelgr_UpdateOpeningAttackCycle(u8 *p)
{
    u8 *q = p;
    struct Action *a = (void *)(p + 0x300);
    if (Start(p)) {
        u16 *counter = (void *)(p + 0x318);
        u32 zero = 0;
        *counter = zero;
        ActionState_Reset(a);
        ActionState_SetState(a, 0);
    }
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Request(q, 14);
                a->frame++;
            } else if (q[0xaf])
                ActionState_AdvanceState(a);
            break;
        case 1: {
            u8 *f = q + 0xa1;
            u32 old = *f;
            u32 bits = 2;
            bits |= old;
            *f = bits;
        }
            if (!a->frame) {
                Request(q, 15);
                a->frame++;
            } else if (q[0xaf])
                ActionState_AdvanceState(a);
            if (TestFlags(q, 0x4000000))
                ActionState_SetState(a, 3);
            break;
        case 2: {
            u8 *f = q + 0xa1;
            u32 old = *f;
            u32 value = 2;
            value |= old;
            value |= 4;
            *f = value;
        }
            if (!a->frame) {
                Request(q, 3);
                a->frame++;
            } else if (q[0xaf]) {
                ActionState_SetState(a, 4);
            }
            if (TestFlags(q, 0x4000000))
                ActionState_SetState(a, 3);
            break;
        case 3:
            if (!a->frame) {
                Request(q, 32);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 4);
            break;
        case 4:
            if (!a->frame) {
                Request(q, 16);
                a->frame++;
            } else {
                u32 mask = 0x40000000;
                u32 flags = *(u32 *)(q + 0x9c);
                if (flags & mask)
                    ActionState_SetState(a, 7);
                else if ((s32)flags < 0)
                    ActionState_SetState(a, 6);
                else if (q[0xaf]) {
                    if ((flags & 0x20000000) && !PlayerFlags(*(u8 **)(q + 0x10c), 0x40000))
                        ActionState_SetState(a, 5);
                    else
                        ActionState_SetState(a, 9);
                }
            }
            break;
        case 5:
            if (!a->frame) {
                Request(q, 17);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 9);
            break;
        case 6:
            ActionState_SetState(a, 7);
            break;
        case 7:
            if (!a->frame) {
                Request(q, 35);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 8);
            break;
        case 8:
            if (!a->frame) {
                Request(q, 24);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 12);
            break;
        case 9:
            if (!a->frame) {
                Request(q, 18);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 10);
            break;
        case 10:
            if (!a->frame) {
                Request(q, 19);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 11);
            break;
        case 11:
            if (!a->frame) {
                Request(q, 1);
                a->frame++;
            }
            if (*(s16 *)(q + 0x56) <= 832)
                ActionState_SetState(a, 12);
            break;
        case 12:
            p[0x383] = 1;
            break;
    }
}
