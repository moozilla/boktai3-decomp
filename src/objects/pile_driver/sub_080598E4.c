#include "objects/pile_driver/pile_driver_rig.h"
#include "objects/pile_driver/pile_driver_rig.h"
static inline s32 PileDriverRig_NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}
void PileDriverRig_TickResourceDrain(struct PileDriverRig *rig)
{
    struct SceneRigParentView *parent = (void *)rig->parent;
    s32 count = PileDriverRig_GetActiveCount(rig), mode = PileDriverRig_GetParentMode(rig);
    if (count > 0 && mode > 0) {
        s32 rate = 0, drain = 0;
        if (rig->state == 2) {
            rate = count * mode * 4;
            if (((PileDriverRig_NextRandom() >> 3) & 1) == 0) {
                SparkParticle_Spawn(&rig->position, 1, rig->payloadA, 0, 0, 0, 2, 1);
            }
        } else if ((u16)(rig->state - 3) <= 2 || rig->state == 7) {
            rate = count * mode;
            if (((PileDriverRig_NextRandom() >> 3) & 7) == 0) {
                SparkParticle_Spawn(&rig->origin, 1, rig->payloadB, 0, 0, 0, 2, 1);
            }
        }
        if (rate > 0) {
            rate = Div(parent->resourceScale * rate, 100);
            if (rate <= 0)
                rate = 1;
        }
        if ((rig->resourceAccumulator += rate) > 40) {
            drain += Div(rig->resourceAccumulator, 40);
            rig->resourceAccumulator -= drain * 40;
        }
        {
            s32 *remaining = &gGameState->unk618;
            if (*remaining <= drain)
                *remaining = 0;
            else
                *remaining -= drain;
        }
        {
            struct SceneRigParentView **global = (void *)&gScenePanel;
            u32 stage = (*global)->warningStage;
            if (stage == 0 && gGameState->unk618 <= gGameState->unk61C >> 2) {
                u16 sound;
                (*global)->warningStage = 1;
                sound = (*global)->warningSounds[(*global)->warningStage];
                if (sound)
                    Sound_PlaySong(sound);
                ScreenTransition_SendCommand(0, 4, 31, 31, 31, 8191, stage);
                PileDriver_CallPendingScript((void *)*global, 4);
            }
        }
    }
}
