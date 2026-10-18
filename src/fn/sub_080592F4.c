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
            panel->handle = Hresvelgr_CreateShotPool(panel->resourceId, 10, 30);
            handleOffset = 0x1364;
            destination = &panel->action;
            action = ScenePanel_TickAngleBurst;
            goto shared;
        case 2:
            panel->handle = SceneEffect_CreateMovingTask(panel->resourceId, &panel->origin);
            handleOffset = 0x1364;
            destination = &panel->action;
            action = ScenePanel_TickHandleActionB;
            goto shared;
        case 3:
            panel->handle = ScenePanelEffect_Create(panel->resourceId, panel->parameter);
            panel->action = ScenePanel_TickHandlePosition;
            panel->interval = 360;
            break;
        case 4:
            panel->handle = SceneEffect_CreateEntryTask(panel->resourceId, &panel->origin);
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
