#include "global.h"
struct HresvelgrActor {
    u8 unknown[0x300];
};
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void SceneMover_SetSideTarget(void *);
s32 SceneMover_IsNearTarget(void *, s32);
void OffsetActor_StepTowardTarget(void *);
void ActionState_Reset(u8 *);
void ActionState_SetState(u8 *, u32);
void ActionState_AdvanceState(u8 *);
void Sound_PlaySong(u32);
void Hresvelgr_PlaySideTargetSequence(struct HresvelgrActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 *state = p + 0x2f0;
    u32 mask = 0x200000;
    u32 *flags = (u32 *)(p + 0x9c);
    u8 callbackResult;
    *flags |= mask;
    callbackResult = Callback_InvokeActor(p, *(void **)(p + 0x104));
    if (callbackResult)
        return;
    if (p[0xb1]) {
        p[0xb1] = callbackResult;
        ActionState_Reset(state);
        ActionState_SetState(state, 0);
    }
    {
        u32 mask = 0x200004;
        *flags |= mask;
    }
    if (p[0xb2])
        Sound_PlaySong(0x3d9);
    switch (*state) {
        case 0:
            if (*(u32 *)(state + 4) == 0) {
                Hresvelgr_SetAnimation(p, p + 0x194, p + 0x174, 0, 1, 0);
                SceneMover_SetSideTarget(p);
                *(u32 *)(state + 4) = 1;
            }
            if (SceneMover_IsNearTarget(p, 4))
                ActionState_AdvanceState(state);
            else
                OffsetActor_StepTowardTarget(p);
            break;
        case 1:
            p[0xaf] = 1;
            break;
    }
}
