#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void ActorSoundPhase_StartTimedAnimation(struct VanargandPairAttack *p)
{
    void *animation = &p->animation;
    u32 mask = 5;
    p->flags = (p->flags | mask) | 0x40;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 3, 4);
        p->delay = 8;
    }
}
