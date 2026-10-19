#include "global.h"
struct HresvelgrVector {
    s16 x, y, z;
};
struct HresvelgrActor {
    u8 unknown[0x174];
    u8 resource[0x20];
    u8 animation[0x2f0 - 0x194];
    u8 action[0x10];
};
void ActionState_Reset(u8 *);
void ActionState_SetState(u8 *, u32);
void ActionState_AdvanceState(u8 *);
void Anim_SetFrameDurationScale(void *, u32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
static inline void SetVector(struct HresvelgrVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void Hresvelgr_PlayAscentSequence(struct HresvelgrActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 *state = actor->action;
    {
        u32 mask = 0x200000;
        *(u32 *)(p + 0x9c) = (*(u32 *)(p + 0x9c) | mask) | 0xc;
    }
    if (p[0xb1]) {
        p[0xb1] = 0;
        ActionState_Reset(state);
        ActionState_SetState(state, 0);
    }
    if (p[0xb2])
        Sound_PlaySong(0x3d9);
    switch (*state) {
        case 0:
            if (*(u32 *)(state + 4) == 0) {
                void *animation = actor->animation;
                Hresvelgr_SetAnimation(p, animation, actor->resource, 0, 1, 0);
                Anim_SetFrameDurationScale(animation, 0x80);
                *(u32 *)(state + 4) = 1;
            }
            if (*(s16 *)(p + 0x56) > 0x1fff) {
                Anim_SetFrameDurationScale(actor->animation, 0x40);
                ActionState_AdvanceState(state);
            } else {
                s32 speed = 0x20;
                SetVector((struct HresvelgrVector *)(p + 0x5c), 0, speed, 0);
            }
            break;
        case 1:
            p[0xaf] = 1;
            break;
    }
}
