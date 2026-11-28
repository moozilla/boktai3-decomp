#include "global.h"
#include "engine/ram.h"
#include "engine/task.h"
#include "ui/pause/scene_hud_display.h"

s32 SceneHud_InitializeOwner(struct SceneHudState *, u32);
void Hud_RefreshPlayerBinding(void);
void Hud_Destroy(void);

struct SceneHudState *Hud_CreateEngineFlagVariant(u32 parameter)
{
    struct SceneHudState *hud;
    if (gFieldHud)
        return (struct SceneHudState *)gFieldHud;
    hud = (struct SceneHudState *)Task_Create(0xc, 0x4DA0);
    if (hud) {
        Task_SetCallbacks(&hud->task, Hud_RefreshPlayerBinding, Hud_Destroy);
        hud->creationParameter = parameter;
        if (SceneHud_InitializeOwner(hud, parameter) < 0) {
            Task_Kill(&hud->task);
            return 0;
        }
    }
    return hud;
}
