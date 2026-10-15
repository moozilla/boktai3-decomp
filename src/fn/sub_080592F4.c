// MATCHING_CONCESSION: register-pinning
#include "scene_panel_schedule.h"
void ScenePanel_InitScheduledAction(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    register void (**destination)(struct ScenePanel *) asm("r1");
    register void (*action)(struct ScenePanel *) asm("r0");
    u32 handleOffset;
    /* Cache the handle offset for the neighboring interval field. */
    switch (panel->resourceIndex) {
        case 0:
            panel->handle = 0;
            handleOffset = 0x1364;
            destination = &panel->action;
            action = ScenePanel_TickHazardSpawner;
            goto shared;
        case 1:
            panel->handle = sub_081A3710(panel->resourceId, 10, 30);
            handleOffset = 0x1364;
            destination = &panel->action;
            action = ScenePanel_TickAngleBurst;
            goto shared;
        case 2:
            panel->handle = sub_0819AD94(panel->resourceId, &panel->origin);
            handleOffset = 0x1364;
            destination = &panel->action;
            action = ScenePanel_TickHandleActionB;
            goto shared;
        case 3:
            panel->handle = sub_08190704(panel->resourceId, panel->parameter);
            panel->action = ScenePanel_TickHandlePosition;
            panel->interval = 360;
            break;
        case 4:
            panel->handle = sub_0819AA8C(panel->resourceId, &panel->origin);
            handleOffset = 0x1364;
            destination = &panel->action;
            action = ScenePanel_TickHandleActionA;
        shared:
            *destination = action;
            {
                u16 *interval;
                u32 duration;
                handleOffset -= 0x16;
                interval = (void *)((u8 *)panel + handleOffset);
                duration = 600;
                *interval = duration;
            }
            break;
        default: {
            register u32 offset asm("r0") = 0x1360;
            asm("" : "+r"(offset));
            destination = (void *)((u8 *)panel + offset);
            action = 0;
            *destination = action;
            break;
        }
    }
}
