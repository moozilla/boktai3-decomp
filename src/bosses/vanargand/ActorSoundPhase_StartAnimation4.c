#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void ActorSoundPhase_StartAnimation4(struct VanargandPairAttack *p)
{
    u32 flag = 4;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        p->attackParameter5B8 = flag;
        p->attackParameter5BA = flag;
        SceneActors_StartUnmirroredAnimation(p, &p->animation, p->drawable, 4, 1, 0);
        Sound_PlaySong(0x586);
    }
    p->delay = 2;
}
