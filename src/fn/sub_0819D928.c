#include "global.h"
#include "ram.h"

// confidence=low: repeats charges until terrain or the attempt limit ends them.

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
s32 Hresvelgr_HasCellOverride(void *);
static inline s32 FacesBoundary(u8 *p)
{
    s32 direction = p[0xcc];
    s32 result;
    if (!p[0x32b]) {
        direction = (direction + 32) & 255;
        if (direction > 64)
            goto outside;
        return 1;
    } else {
        direction = (direction - 32) & 255;
        result = 1;
        if (direction > 64) {
        outside:
            result = 0;
        }
    }
    return result;
}
void Hresvelgr_UpdateRepeatedCharge(u8 *p)
{
    u8 *q = p;
    struct Action *a = (void *)(p + 0x300);
    if (Start(p)) {
        *(u16 *)(p + 0x330) = 0;
        ActionState_Reset(a);
        ActionState_SetState(a, 0);
    }
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Request(q, 24);
                a->frame++;
            } else if (q[0xaf])
                ActionState_AdvanceState(a);
            break;
        case 1:
            if (!a->frame) {
                if (Hresvelgr_HasCellOverride(q)) {
                    ActionState_SetState(a, 3);
                    break;
                }
                Request(q, 26);
                (*(u16 *)(p + 0x330))++;
                a->frame++;
            } else if (q[0xaf]) {
                if (*(u16 *)(p + 0x330) > 3)
                    ActionState_SetState(a, 4);
                else if (FacesBoundary(q))
                    ActionState_SetState(a, 1);
                else
                    ActionState_SetState(a, 2);
            }
            break;
        case 2:
            if (!a->frame) {
                Request(q, 2);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 1);
            break;
        case 3:
            if (!a->frame) {
                Request(q, 25);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 1);
            break;
        case 4:
            if (!a->frame) {
                Request(q, 12);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 5);
            break;
        case 5:
            p[0x383] = 1;
            break;
    }
}
