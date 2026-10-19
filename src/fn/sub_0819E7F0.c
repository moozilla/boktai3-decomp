#include "global.h"

/* tentative: rises, then dives until it lands and spawns a ground burst */
#define Hresvelgr_UpdateGroundSlam sub_0819E7F0

struct BurstVector {
    s16 x, y, z, pad;
};
struct HresvelgrDive {
    u8 unk0[0x51];
    u8 angle;
    u8 unk52[2];
    struct BurstVector position;
    u8 unk5C[0x9C - 0x5C];
    u32 flags;
    u8 unkA0[8];
    u8 direction4, direction8;
    u8 unkAA[5];
    u8 done;
    u8 unkB0;
    u8 pending;
    u8 landed;
    u8 unkB3[0xCC - 0xB3];
    u8 requestedAngle;
    u8 unkCD[0x104 - 0xCD];
    void *callback;
    u8 unk108[0x174 - 0x108];
    u8 animationResource[0x20];
    u8 animation[0x2F0 - 0x194];
    u8 action[8];
    u8 unk2F8[0x32B - 0x2F8];
    u8 variant;
};
void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void ActionState_Reset(u8 *);
void ActionState_SetState(u8 *, u32);
void sub_0819C8F8(void *, u32, u32);
void EffectPool_CreateEntry(void *, u32, u32, u32, u32);
static inline void SetVector(struct BurstVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void Hresvelgr_UpdateGroundSlam(struct HresvelgrDive *p)
{
    u8 *action = p->action;
    struct BurstVector pos;
    u32 mask = 0x600040;
    u32 *flags = &p->flags;
    *flags |= mask;
    if (p->pending) {
        p->pending = 0;
        ActionState_Reset(action);
        p->angle = p->requestedAngle;
        {
            s32 angle = (p->angle + 0x60) >> 6;
            u32 direction = 3;
            direction &= angle;
            p->direction4 = direction;
        }
        {
            s32 angle = (p->angle + 0x70) >> 5;
            u32 direction = 7;
            direction &= angle;
            p->direction8 = direction;
        }
    }
    switch (*action) {
        case 0:
            if (*(u32 *)(action + 4) == 0) {
                Hresvelgr_SetAnimation(p, p->animation, p->animationResource, 0, 1, 0);
                (*(u32 *)(action + 4))++;
            } else if (*(u32 *)(action + 4) > 0x40)
                ActionState_SetState(action, 1);
            else
                (*(u32 *)(action + 4))++;
            break;
        case 1:
            if (*(u32 *)(action + 4) == 0)
                Hresvelgr_SetAnimation(p, p->animation, p->animationResource, 0x13, 2, 4);
            else {
                u32 mask = 0x800000;
                *flags |= mask;
            }
            if (++(*(u32 *)(action + 4)) > 0x10)
                ActionState_SetState(action, 2);
            break;
        case 2:
            if (*(u32 *)(action + 4) == 0) {
                Sound_PlaySong(0x3E2);
                (*(u32 *)(action + 4))++;
            }
            sub_0819C8F8(p, 0x10, *(u32 *)(action + 4) << 2);
            if (p->landed) {
                s32 x = p->position.x;
                s32 z = p->position.z;
                SetVector(&pos, x, 0x200, z);
                EffectPool_CreateEntry(&pos, p->variant << 6, 10, 4, 0x20);
                Sound_PlaySong(0x3DA);
                ActionState_SetState(action, 3);
            } else
                (*(u32 *)(action + 4))++;
            break;
        case 3:
            p->done = 1;
            break;
    }
    Callback_InvokeActor(p, p->callback);
}
