#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdateAnimationGate(struct VanargandPairAttack *p, s32 frame)
{
    struct VanargandBodyAnimation *anim = &p->animation;
    u8 phase;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        p->delay = 16;
        p->phase = 0;
        p->timer = 0;
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            if (frame == 16)
                SceneActors_StartUnmirroredAnimation(p, anim, p->drawable, 15, 2, 4);
            if (frame > 16 && p->animationEvent) {
                p->phase = 1;
                p->timer = phase;
                {
                    u16 v = 4;
                    p->attackParameter5B8 = v;
                    p->attackParameter5BA = v;
                }
                return;
            }
            break;
        case 1:
            p->finished = phase;
            return;
    }
    {
        u32 mask = 5;
        p->flags |= mask;
    }
}
