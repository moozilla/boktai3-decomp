#define SceneHud_ShowActionPreview sub_0816D868
#define SceneHud_GetSelectedAction Inventory_GetQuantityByStorage
#include "global.h"
#include "scene_hud_display.h"
#include "scene_inventory_panel.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
void ScrollingDialogue_RequestShow(void);
void PauseMenu_ClearSpriteFlags(struct SceneHudState *, s32, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
void sub_0816D868(struct SceneHudState *s)
{
    s32 action, keyframe;
    s32 text;
    if (s->actionCursor <= 15) {
        action = Inventory_GetQuantityByStorage(s->actionCategory, s->actionCursor);
        if (s->actionCategory == 0)
            keyframe = sub_0816D820(s->actionCursor);
        else
            keyframe = 394;
    } else {
        action = -1;
        keyframe = 394;
    }
    if (action < 0) {
        s->actionAnimation.flags |= 1;
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
    } else {
        PauseMenu_ClearSpriteFlags(s, 59, 1);
        Anim_LoadKeyframe(&s->actionAnimation, s->motionResource, (u16)keyframe, 1);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(s->actionTextScript);
        MessageWindow_SetBounds(2, 16, 28, 2);
        MessageWindow_ShowMessage(action);
        ScrollingDialogue_RequestShow();
        text = Script_ParseStringRef(s->actionTextScript);
        text += 55;
        text += action;
        ScrollingDialogue_SetScript(Text_LookupString(text));
    }
}
