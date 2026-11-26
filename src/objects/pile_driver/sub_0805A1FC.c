#include "objects/pile_driver/pile_driver_rig.h"
static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}
void PileDriverRig_TickSubjectAction(struct ScenePanelRig *rig)
{
    s8 *selected = &rig->selectedSubject;
    if (PileDriverRig_IsSubjectState(rig, *selected, 4)) {
        if (((randomValue() >> 3) & 3) == 0)
            SparkParticle_Spawn(&rig->position, 1, rig->payloadA, 0, 0, 0, 1, 2);
        if (++rig->stateFrame > 89) {
            PileDriverRig_SetSubjectState(rig, *selected, 5);
            PileDriverRig_RandomizeMovement(rig);
            PileDriverRig_SetState(rig, 3);
        } else
            PileDriverRig_RequestSubject(rig, *selected, 5);
    } else {
        PileDriverRig_RandomizeMovement(rig);
        PileDriverRig_SetState(rig, 3);
    }
}
