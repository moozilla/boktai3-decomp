#include "global.h"
struct OffsetActor {
    u8 unknown[0x300];
};
void ActionState_Reset(u8 *);
void ActionState_SetState(u8 *, u32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void sub_0819E754(struct OffsetActor *, u32);
static inline u8 OutsideBoundary(struct OffsetActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 outside = 0;
    if (*(s16 *)(p + 0x54) > 0x2000 || *(s16 *)(p + 0x58) > 0x2000)
        outside = 1;
    return outside;
}
void OffsetActor_PlayBoundarySequence(struct OffsetActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 *state = p + 0x2f0;
    if (p[0xb1]) {
        p[0xb1] = 0;
        ActionState_Reset(state);
    }
    {
        u32 mask = 0x20000;
        *(u32 *)(p + 0x9c) |= mask;
    }
    switch (*state) {
        case 0:
            if (*(u32 *)(state + 4) == 0) {
                Hresvelgr_SetAnimation(p, p + 0x194, p + 0x174, 2, 1, 0);
                *(u32 *)(state + 4) = 1;
            }
            sub_0819E754(actor, 0x20);
            if (OutsideBoundary(actor))
                ActionState_SetState(state, 1);
            break;
        case 1:
            p[0xaf] = 1;
            break;
    }
}
