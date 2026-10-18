#define SceneInput_SetWidgetPalette PauseMenu_SetSpritePalette
#include "global.h"
#include "scene_hud_display.h"
s32 GameState_GetUnlockEntry(s32);
s32 SolarFruit_GetInventorySlot(u32);
void SceneHud_SetActionCursorPosition(void *, u32);
void PauseMenu_SetSpritePalette(void *, u16);
void SceneHud_LoadActionMotion(struct SceneHudState *p, u32 index, u32 source, u32 target, u32 kind)
{
    struct SceneInputWidget *w = &p->motionWidgets[index];
    w->flags &= ~1;
    if (kind == 0) {
        void *data = p->motionResource;
        u16 frame = SolarFruit_GetInventorySlot(source) + 0xe7;
        Anim_LoadKeyframe(w, data, frame, 1);
    } else if (kind == 1) {
        void *data = p->motionResource;
        u16 frame = GameState_GetUnlockEntry(source - 8) + 0xd9;
        Anim_LoadKeyframe(w, data, frame, 1);
    } else
        return;
    SceneHud_SetActionCursorPosition(&w->position, source);
    SceneHud_SetActionCursorPosition(&p->actionTargets[index], target);
    PauseMenu_SetSpritePalette(w, p->categoryWidgets[source + 10].paletteBank);
}
