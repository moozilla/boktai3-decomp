#define SceneHud_FinishRandomActionReturn sub_0816DD70
#define SceneHud_RefreshActionIcons sub_0816D98C
#define SceneHud_GetActionIconPosition sub_0816D920
#define SceneItem_GetSlotItem Inventory_GetItemId
#define SceneHud_TickIconMotion PauseMenu_UpdateSpriteMotion
#include "global.h"
#include "scene_hud_display.h"
u32 SceneHud_TickIconMotion(void *, u32);
s32 SceneItem_GetSlotItem(u32);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_GetActionIconPosition(void *, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void sub_0816FF4C(void);
void sub_0816CDB8(void *, s32, s32);
void sub_0816D868(void *);
void Hud_SaveGridSelection(void *);
void sub_081672FC(void *);
void sub_08166B7C(void *, s32);
void sub_0816701C(void *, s32, s32, s32, s32, s32);
void sub_081663A0(void *, s32, s32, s32, s32);
void sub_0816DD70(struct SceneHudState *s)
{
    void *input;
    if (SceneHud_TickIconMotion(s, 0)) {
        if (SceneItem_GetSlotItem(s->actionSelection) == 31)
            Sound_PlaySong(0x123);
        *(u32 *)((u8 *)s + 0x2aa8) |= 1;
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, sub_0816FF4C, 1);
        input = s->actionInput;
        sub_0816CDB8(input, 1, 0);
        SceneHud_GetActionIconPosition(&s->categoryWidgets[5].position, s->actionCursor);
        sub_0816D868(s);
        Hud_SaveGridSelection(input);
        sub_081672FC(s);
        sub_08166B7C(s, 4);
        sub_0816701C(s, 0, 0, 0, 1, 0);
        sub_081663A0(s, 1, 0, 0, 0);
    }
}
