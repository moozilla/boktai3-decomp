// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[4];
    u8 state, finished, enter, changed;
    u8 padB2[2];
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[0x224];
    u8 animation[0x20];
    u8 animationData[0x60];
    u8 polar[0xab0];
    u8 target[0x10];
    u16 count;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void PolarInterpolation_SetCurrentFromPoint(void *, void *);
void sub_082222BC(void *, void *, int);
int Interp_Step(void *, void *);
void Sound_PlaySong(int);
int ActorResource_GetFixedId(void *);
int Callback_InvokeActor(void *, void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);
void SceneActorAnimation_UpdateSwoopApproach(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    setFacing(p, p->requestedAngle);
    if (p->changed) {
        p->changed = 0;
        {
            int off = 0x34c;
            u8 *data;
            u8 *anim;
            data = (u8 *)p + off;
            anim = p->animation;
            SceneActors_StartEightDirectionAnimation(p, data, anim, 2, 1, 1, 4);
        }
        PolarInterpolation_SetCurrentFromPoint(p->polar, (u8 *)p + 0x54);
        {
            int off2 = 0xe5c;
            u8 *dst;
            u16 *count;
            dst = (u8 *)p + off2;
            {
                int off3 = 0xe6c;
                count = (u16 *)((u8 *)p + off3);
            }
            sub_082222BC(p->polar, dst, *count);
            if (*count <= 64)
                Sound_PlaySong(0x567);
            else
                Sound_PlaySong(0x56c);
        }
    } else {
        {
            int off = 0x34c;
            u8 *data;
            u8 *anim;
            data = (u8 *)p + off;
            anim = p->animation;
            SceneActors_StartEightDirectionAnimation(p, data, anim, 2, 1, 1, 1);
        }
    }
    if (!(u8)Callback_InvokeActor(p, p->callback)) {
        if (!(tick & 3)) {
            int id = ActorResource_GetFixedId(p2);
            {
                u8 *data = gPaletteBankData + id * 32;
                TemporaryAnimationPool_CreateEntry(p2->animationData, 8, 0x200, 0xe011, data);
            }
        }
        if (p2->count <= 0x20) {
            u32 mask = 0x40000;
            u32 v = p->flags;
            v = v | mask;
            v = v | 8;
            v = v | 4;
            p->flags = v;
        }
        if (p2->count > 0x3f && p->state != 8 && p->state != 4) {
            u32 mask = 0x40;
            p->flags = p->flags | mask;
        }
        if (Interp_Step(p2->polar, position + 12))
            p->finished = 1;
    }
}
