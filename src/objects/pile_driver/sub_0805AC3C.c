#include "objects/pile_driver/pile_driver_rig.h"
#include "objects/pile_driver/pile_driver_schedule.h"
extern void *gUnk_08605A94[];
void PileDriverRig_CheckContactTransition(struct ScenePanelRig *);
void PileDriverRig_TickResourceDrain(struct ScenePanelRig *);
void PileDriverRig_TickSprites(struct ScenePanelRig *, u32);
void PileDriver_TickRig(struct ScenePanel *input)
{
    struct ScenePanel *parent = input;
    struct ScenePanelRig *rig = &parent->rig;
    u32 palette;
    PileDriverRig_CheckContactTransition(rig);
    Callback_InvokeActor(rig, gUnk_08605A94[parent->rig.state]);
    if (parent->rig.colliderHold) {
        u16 *flags = &parent->rig.collider.flags;
        u32 value = *flags;
        u32 mask = 4;
        mask |= value;
        *flags = mask;
        parent->rig.colliderHold--;
    } else {
        u16 *flags = &parent->rig.collider.flags;
        u32 value = *flags;
        u32 mask = 0xFFFB;
        mask &= value;
        *flags = mask;
        Collider_SetPosition(&parent->rig.collider, &parent->rig.position, 0);
    }
    if (!parent->skipMainSprite)
        PileDriverRig_TickResourceDrain(rig);
    palette = 91;
    {
        struct ScenePanelScheduleView *schedule = (void *)parent;
        if (schedule->action) {
            if (schedule->frame >= schedule->interval)
                palette = 92;
            else if (schedule->frame >= schedule->interval - 90 && ((schedule->frame >> 2) & 1))
                palette = 92;
        }
    }
    PileDriverRig_TickSprites(rig, palette);
}
