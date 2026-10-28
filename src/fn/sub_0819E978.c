#include "global.h"
#include "ram.h"

/* tentative: travels to the nearest active growth hazard, lands on it and resets its cooldown */

typedef struct HresvelgrPosition {
    s16 x, y, z, pad;
} HresvelgrPosition;
typedef struct HresvelgrActor {
    u8 pad0[0x54];
    HresvelgrPosition position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[2];
    u8 state;
    u8 animationCompleted;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 landed;
    u8 padB3[0x104 - 0xB3];
    void *callback;
    u8 pad108[0x174 - 0x108];
    u8 animationResource[0x20];
    u8 animation[0x39C - 0x194];
    HresvelgrPosition target;
    u8 pad3A4[0x420 - 0x3A4];
    void *hazards;
} HresvelgrActor;
void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
s32 GrowthHazard_FindNearestActiveEntry(void *, HresvelgrPosition *, HresvelgrPosition *);
s32 ColliderActor_SetTargetPosition(HresvelgrActor *, HresvelgrPosition *);
s32 Hresvelgr_UpdateTravel(HresvelgrActor *);
void Anim_SetFrameDurationScale(void *, u32);
void GrowthHazard_ResetEntryCooldown(void *, s32);
void Hresvelgr_UpdateHazardVisit(HresvelgrActor *p)
{
    HresvelgrActor *self = p;
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        gHresvelgrHazardIndex =
            GrowthHazard_FindNearestActiveEntry(p->hazards, &p->position, &p->target);
        if (gHresvelgrHazardIndex >= 0) {
            Hresvelgr_SetAnimation(p, p->animation, p->animationResource, 0, 1, 4);
            ColliderActor_SetTargetPosition(p, &p->target);
            p->state = 0;
            p->timer = 0;
        } else {
            u8 st = 2;
            p->state = st;
            p->timer = 0;
        }
    }
    if (p->landed)
        Sound_PlaySong(0x3D9);
    switch (p->state) {
        case 0:
            if (Hresvelgr_UpdateTravel(self)) {
                p->state = 1;
                p->timer = 0;
            }
            break;
        case 1:
            if (p->timer == 0) {
                Hresvelgr_SetAnimation(self, self->animation, self->animationResource, 0x13, 1, 0);
                Anim_SetFrameDurationScale(self->animation, 0x20);
                p->timer++;
            }
            if (p->landed && p->animationCompleted > 4) {
                GrowthHazard_ResetEntryCooldown(self->hazards, gHresvelgrHazardIndex);
                Anim_SetFrameDurationScale(self->animation, 0x40);
                {
                    u8 st = 2;
                    u32 zero = 0;
                    p->state = st;
                    p->timer = zero;
                }
            }
            break;
        case 2:
            p->finished = 1;
            break;
    }
    Callback_InvokeActor(p, p->callback);
}
