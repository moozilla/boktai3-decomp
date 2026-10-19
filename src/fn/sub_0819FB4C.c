#include "global.h"
struct TargetVector {
    s16 x, y, z;
};
struct OffsetActor {
    u8 unknown[0x174];
    u8 motion[0x20];
    u8 reaction[0x208];
    struct TargetVector target;
};
void ActionState_Reset(u8 *);
void ActionState_AdvanceState(u8 *);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void OffsetActor_StepTowardTarget(struct OffsetActor *);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
static inline void SetTarget(struct TargetVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void OffsetActor_PlayTargetSequence(struct OffsetActor *object)
{
    u8 *p = (u8 *)object;
    struct OffsetActor *actor = object;
    u8 *state = p + 0x2f0;
    if (p[0xb1]) {
        u32 zero = 0;
        p[0xb1] = zero;
        Hresvelgr_SetAnimation(p, object->reaction, object->motion, zero, 1, zero);
        ActionState_Reset(state);
    }
    {
        u32 mask = 0x600040;
        *(u32 *)(p + 0x9c) |= mask;
    }
    if (p[0xb2])
        Sound_PlaySong(0x3d9);
    switch (*state) {
        case 0:
            if (*(u32 *)(state + 4) > 0x80)
                ActionState_AdvanceState(state);
            else {
                s16 *v = *(s16 **)(p + 0x110);
                if (!p[0x32b]) {
                    s32 y = v[1], z = v[2];
                    struct TargetVector *target = &object->target;
                    SetTarget(target, 0x400, y, z);
                } else {
                    s32 x = v[0], y = v[1];
                    struct TargetVector *target = &object->target;
                    SetTarget(target, x, y, 0x400);
                }
                OffsetActor_StepTowardTarget(actor);
                (*(u32 *)(state + 4))++;
            }
            break;
        case 1:
            p[0xaf] = 1;
            break;
    }
    Callback_InvokeActor(p, *(void **)(p + 0x104));
}
