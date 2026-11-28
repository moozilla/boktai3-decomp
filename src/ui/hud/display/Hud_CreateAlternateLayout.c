#include "global.h"
#include "engine/ram.h"
#include "engine/task.h"
#include "ui/pause/scene_hud_display.h"

s32 Hud_InitAlternateLayout(struct SceneHudState *, u32);
void Hud_UpdateWithCallback(void);
void SceneHud_DestroyOwner(void);

struct SceneHudState *Hud_CreateAlternateLayout(u32 parameter)
{
    struct SceneHudState *hud;
    if (gFieldHud)
        return (struct SceneHudState *)gFieldHud;
    hud = (struct SceneHudState *)Task_Create(0xc, 0x4DA0);
    if (hud) {
        Task_SetCallbacks(&hud->task, Hud_UpdateWithCallback, SceneHud_DestroyOwner);
        hud->creationParameter = parameter;
        if (Hud_InitAlternateLayout(hud, parameter) < 0) {
            Task_Kill(&hud->task);
            return 0;
        }
    }
    return hud;
}
