#include "bosses/vanargand/vanargand_pair_attack.h"

void Anim_SetFrameDurationScale(void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdateTimedAnimationAttack(struct VanargandPairAttack *p)
{
    u32 value;
    struct VanargandBodyAnimation *animation = &p->animation;
    u32 mask = 0x40;
    u32 *flags = &p->flags;
    value = *flags | mask;
    *flags = value;
    Vanargand_SetPendingCue(p, 1);
    value = p->callback(p);
    if (value)
        return;
    if (p->pending) {
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 15, 2, 2);
        Anim_SetFrameDurationScale(animation, 0x100);
        {
            u32 one = 1;
            u32 five = 5;
            p->attackParameter5B8 = one;
            p->attackParameter5BA = five;
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 0x13);
            p->phase = 0;
            p->timer = 0;
        }
        Sound_PlaySong(0x591);
    }
    switch (p->phase) {
        case 0:
            value = 24;
            if (++p->timer > value) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1: {
            u32 flag = 0x04000000;
            *flags |= flag;
        }
            if (++p->timer > 240) {
                animation->flags &= ~2;
                p->phase = 2;
                p->timer = 0;
            }
            break;
        case 2:
            if (p->animationEvent) {
                p->phase = 3;
                p->timer = value;
                p->finished = 1;
            }
            break;
    }
}
