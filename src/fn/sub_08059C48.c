// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}
static inline s32 randomFirst(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline s32 quarter(s32 value)
{
    return value >= 0 ? value >> 2 : -((-value) >> 2);
}
void ScenePanelRig_TickHoverSelection(struct ScenePanelRig *rig)
{
    register u32 observed asm("r4");
    s32 sample;
    rig->colliderHold = 16;
    rig->position.x = quarter(rig->position.x * 3 + rig->origin.x);
    observed = 0x0403B714;
    sample = randomFirst();
    {
        u16 *originY = (u16 *)&rig->origin.y;
        sample = (sample & 63) + 128;
        rig->position.y = *originY + sample;
    }
    rig->position.z = quarter(rig->position.z * 3 + rig->origin.z);
    asm volatile("" ::"r"(observed));
    if (rig->stateFrame < 300)
        rig->stateFrame++;
    if (rig->stateFrame >= 60) {
        if (--rig->selectionTimer == 0) {
            rig->selectionTimer = Mod(randomValue(), 300) + 300;
            {
                s32 subject = ScenePanelRig_SelectActiveSubject(rig);
                register u32 scratch asm("r1");
                asm("" : "=r"(scratch));
                rig->selectedSubject = subject;
                asm("" ::"r"(scratch));
            }
            if (rig->selectedSubject >= 0) {
                ScenePanelRig_InitMovement(rig);
                if (rig->selectedSubject == 0)
                    rig->angle = 1920;
                else if (rig->selectedSubject == 1)
                    rig->angle = 3968;
                else if (rig->selectedSubject == 2)
                    rig->angle = 3200;
                else
                    rig->angle = 1152;
                ScenePanelRig_SetState(rig, 4);
            }
        } else if (rig->stateFrame >= 300) {
            ScenePanelRig_RandomizeMovement(rig);
            rig->speed = 448;
            rig->verticalStep = 96;
            ScenePanelRig_SetState(rig, 3);
        }
    }
}
