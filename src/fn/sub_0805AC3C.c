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
    /* Allocation-only: this unreachable signed-load arm reserves reload scratch. */
    {
        u32 raw = parent->skipMainSprite;
        if ((s32)raw < 0) {
            u32 value = parent->rig.state;
            value += parent->rig.turnStep + parent->rig.turnRate;
            parent->rig.state = value;
        }
    }
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
            u16 *framePtr = &schedule->frame;
            u16 *intervalPtr = &schedule->interval;
            u32 first = *(volatile u16 *)framePtr;
            /* Reuse this timer word for the second frame sample. */
            u32 interval = *intervalPtr;
            /* Allocation-only: the bounded fallback retains the timer preferences. */
            if ((s32)interval < 0) {
                Anim_LoadKeyframe(rig, intervalPtr, 0, interval);
                goto done;
            }
            if (first >= interval)
                palette = 92;
            else {
                interval = *framePtr;
                if ((s32)interval >= (s32) * (volatile u16 *)intervalPtr - 90 &&
                    ((interval >> 2) & 1))
                    palette = 92;
            }
        }
    }
done:
    ScenePanelRig_TickSprites(rig, palette);
}
