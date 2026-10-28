#include "global.h"
#include "ram.h"

// confidence=low: requests movement around adjacent terrain overrides.

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
s32 Hresvelgr_HasAdjacentOverride(void *);
void Hresvelgr_UpdateTerrainManeuver(u8 *p)
{
    u8 *q = p;
    struct Action *a = (void *)(p + 0x300);
    if (Start(p))
        ActionState_Reset(a);
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Request(q, 24);
                a->frame++;
            } else if (q[0xaf]) {
                if (Hresvelgr_HasAdjacentOverride(q))
                    ActionState_SetState(a, 1);
                else
                    ActionState_SetState(a, 2);
            }
            break;
        case 1:
            if (!a->frame) {
                Request(q, 25);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 2);
            break;
        case 2:
            if (!a->frame) {
                Request(q, 3);
                a->frame++;
            } else if (q[0xaf])
                ActionState_AdvanceState(a);
            break;
        case 3:
            if (!a->frame) {
                Request(q, 4);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 5);
            break;
        case 4:
            if (!a->frame) {
                Request(q, 6);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 6);
            break;
        case 5:
            if (!a->frame) {
                Request(q, 5);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 4);
            break;
        case 6:
            if (!a->frame) {
                Request(q, 7);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 7);
            break;
        case 7:
            if (!a->frame) {
                Request(q, 12);
                a->frame++;
            } else if (q[0xaf])
                ActionState_SetState(a, 8);
            break;
        case 8:
            p[0x383] = 1;
            break;
    }
}
