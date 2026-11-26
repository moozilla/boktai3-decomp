#include "objects/pile_driver/pile_driver_rig.h"
#include "engine/math/monster_particle_random.h"
static inline s32 quarter(s32 value)
{
    return value >= 0 ? value >> 2 : -((-value) >> 2);
}
void PileDriverRig_TickHoverSelection(struct PileDriverRig *rig)
{
    s32 sample;
    u32 origin;
    rig->colliderHold = 16;
    rig->position.x = quarter(rig->position.x * 3 + rig->origin.x);
    origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
    sample = ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR);
    {
        u16 *originY = (u16 *)&rig->origin.y;
        sample = (sample & 63) + 128;
        rig->position.y = *originY + sample;
    }
    rig->position.z = quarter(rig->position.z * 3 + rig->origin.z);
    if (rig->stateFrame < 300)
        rig->stateFrame++;
    if (rig->stateFrame >= 60) {
        if (--rig->selectionTimer == 0) {
            rig->selectionTimer =
                Mod(ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR),
                    300) +
                300;
            rig->selectedSubject = PileDriverRig_SelectActiveSubject(rig);
            if (rig->selectedSubject >= 0) {
                PileDriverRig_InitMovement(rig);
                if (rig->selectedSubject == 0)
                    rig->angle = 1920;
                else if (rig->selectedSubject == 1)
                    rig->angle = 3968;
                else if (rig->selectedSubject == 2)
                    rig->angle = 3200;
                else
                    rig->angle = 1152;
                PileDriverRig_SetState(rig, 4);
            }
        } else if (rig->stateFrame >= 300) {
            PileDriverRig_RandomizeMovement(rig);
            rig->speed = 448;
            rig->verticalStep = 96;
            PileDriverRig_SetState(rig, 3);
        }
    }
}
