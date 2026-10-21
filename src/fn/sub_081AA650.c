// MATCHING_CONCESSION: register-pinning
#include "global.h"

// foundry draft (worker best 88.0%); what differs is mostly register allocation and instruction order
struct VanargandAnimation {
    u8 pad0[0x18];
    u16 flags;
    u8 pad1A[0x60 - 0x1A];
};
struct Vanargand {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 phaseTimer;
    u8 padA8[7];
    u8 finished, entered, pendingAnimation, animationEvent;
    u8 padB3[0x104 - 0xB3];
    void *callback;
    u8 pad108[0x18C - 0x108];
    struct VanargandAnimation animation;
    u8 drawable[0x56C - 0x1EC];
    void *attachment;
    u8 pad570[0x5B8 - 0x570];
    u16 f5B8, f5BA;
};
void Anim_SetFrameDurationScale(void *, u32);
s32 Callback_InvokeActor(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdateTimedAnimationAttack(struct Vanargand *p)
{
    u32 result;
    struct VanargandAnimation *animation = &p->animation;
    register s32 phase asm("r2");
    u32 mask = 0x40;
    register u32 *fp asm("r6") = &p->flags;
    *fp |= mask;
    Vanargand_SetPendingCue(p, 1);
    result = (u8)Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 15, 2, 2);
        Anim_SetFrameDurationScale(animation, 0x100);
        {
            u32 one = 1;
            u32 five = 5;
            u32 zero = 0;
            p->f5B8 = one;
            p->f5BA = five;
            PairedSceneActor_ApplyToSelectedHalves(p->attachment, 3, 0x13);
            p->phase = zero;
            p->phaseTimer = result;
        }
        Sound_PlaySong(0x591);
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            p->phaseTimer++;
            if (p->phaseTimer > 24) {
                p->phase = 1;
                p->phaseTimer = phase;
            }
            break;
        case 1: {
            u32 flag = 0x04000000;
            *fp |= flag;
        }
            p->phaseTimer++;
            if (p->phaseTimer > 240) {
                animation->flags &= 0xFFFD;
                p->phase = 2;
                p->phaseTimer = result;
            }
            break;
        case 2:
            if (p->animationEvent) {
                p->phase = 3;
                p->phaseTimer = result;
                p->finished = 1;
            }
            break;
    }
}
