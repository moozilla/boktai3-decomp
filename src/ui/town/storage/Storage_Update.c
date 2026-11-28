#include "global.h"
#include "ui/town/storage_screen.h"
void SceneHud_DrawNumericStatus(u32);
void SceneHud_DrawClock(u32);
void Storage_Update(struct StorageScreen *screen)
{
    SceneHud_DrawNumericStatus(screen->categoryKinds[6]);
    SceneHud_DrawClock(screen->categoryKinds[6]);
    Callback_InvokeActor(screen, screen->updateCallback);
}
