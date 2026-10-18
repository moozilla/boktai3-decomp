#define SceneHud_ApplyTimedSceneMode sub_0816D114
#define SceneHud_GetSelectedAction Inventory_GetQuantityByStorage
#include "global.h"
#include "player.h"
#include "scene_hud_display.h"
s32 Inventory_GetQuantityByStorage(u32, u32);
void sub_0816D114(struct SceneHudState *s, s32 mode, s32 duration)
{
    struct Player *p = s->actionOwner;
    if (p) {
        p->unk536 = Inventory_GetQuantityByStorage(s->actionCategory, s->actionSelection);
        p->sceneMode = mode;
        p->active = duration * 60;
        p->actionEighteenParameter = 366;
        p->actionEighteenDuration = 48;
        s->appliedStatB = 0;
        s->actionRefreshRequested = 1;
        s->statusRefreshRequested = 1;
    }
}
