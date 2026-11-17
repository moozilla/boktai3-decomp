#include "scene_panel_rig.h"
#include "scene_panel_schedule.h"
extern void *gUnk_08605A94[];
void ScenePanelRig_CheckContactTransition(struct ScenePanelRig *);
void ScenePanelRig_TickResourceDrain(struct ScenePanelRig *);
void ScenePanelRig_TickSprites(struct ScenePanelRig *, u32);
void ScenePanel_TickRig(struct ScenePanel *input)
{
    struct ScenePanel *parent = input;
    struct ScenePanelRig *rig = &parent->rig;
    u32 palette;
    ScenePanelRig_CheckContactTransition(rig);
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
        ScenePanelRig_TickResourceDrain(rig);
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
    ScenePanelRig_TickSprites(rig, palette);
}
