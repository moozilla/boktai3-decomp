#include "vanargand_pair_attack.h"

void Anim_SetFrameDurationScale(void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdateTimedAnimationAttack(struct VanargandPairAttack *p)
{
    union {
        u32 flags, callbackResult, frameLimit;
    } value;
    struct VanargandBodyAnimation *animation = &p->animation;
    s32 phase;
    u32 mask = 0x40;
    u32 *flags = &p->flags;
    value.flags = *flags | mask;
    *flags = value.flags;
    Vanargand_SetPendingCue(p, 1);
    value.callbackResult = p->callback(p);
    if (value.callbackResult)
        return;
    if (p->pending) {
        p->pending = value.callbackResult;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 15, 2, 2);
        Anim_SetFrameDurationScale(animation, 0x100);
        {
            u32 one = 1;
            u32 five = 5;
            u32 zero = 0;
            p->attackParameter5B8 = one;
            p->attackParameter5BA = five;
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 0x13);
            p->phase = zero;
            p->timer = value.callbackResult;
        }
        Sound_PlaySong(0x591);
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            p->timer++;
            value.frameLimit = 24;
            if (p->timer > value.frameLimit) {
                p->phase = 1;
                p->timer = phase;
            }
            break;
        case 1: {
            u32 flag = 0x04000000;
            *flags |= flag;
        }
            p->timer++;
            if (p->timer > 240) {
                animation->flags &= 0xFFFD;
                p->phase = 2;
                p->timer = value.callbackResult;
            }
            break;
        case 2:
            if (p->animationEvent) {
                p->phase = 3;
                p->timer = value.callbackResult;
                p->finished = 1;
            }
            break;
    }
}
