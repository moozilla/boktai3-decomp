#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_SetPendingCue(struct VanargandPairAttack *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
s32 Callback_InvokeActor(void *, void *);

void ActorSoundPhase_StartFlaggedAnimation(struct VanargandPairAttack *boss)
{
    u32 flag = 0x40;

    boss->flags |= flag;
    Vanargand_SetPendingCue(boss, 0);
    if (boss->pending) {
        boss->pending = 0;
        SceneActors_StartUnmirroredAnimation(boss, &boss->animation, boss->drawable, 2, 1, 4);
        PairedSceneActor_ApplyToSelectedHalves(boss->pair, 3, 2);
    }
    Callback_InvokeActor(boss, boss->callback);
}
