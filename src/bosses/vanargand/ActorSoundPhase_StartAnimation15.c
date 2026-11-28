#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Anim_SetFrameDurationScale(void *, s32);
void ActorSoundPhase_StartAnimation15(struct VanargandPairAttack *p)
{
    u32 flag = 4;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        void *animation;
        u32 one, two;
        p->pending = 0;
        one = 1;
        two = 2;
        p->attackParameter5B8 = one;
        p->attackParameter5BA = two;
        animation = &p->animation;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 15, two, 0);
        Anim_SetFrameDurationScale(animation, 0x80);
    }
}
