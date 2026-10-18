#define GameState_GetActionDescriptionIndex SolarFruit_GetInventorySlot
#define SceneHud_ClearWidgetFlags PauseMenu_ClearSpriteFlags
#include "global.h"
#include "scene_hud_display.h"
#include "scene_inventory_panel.h"
s32 GameState_GetUnlockEntry(s32);
s32 SolarFruit_GetInventorySlot(s32);
void ScrollingDialogue_RequestShow(void);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void SceneHud_ShowSelectionDescription(struct SceneHudState *p)
{
    s32 entry;
    s32 text;
    if (p->actionCursor <= 7) {
        entry = SolarFruit_GetInventorySlot(p->actionCursor);
        if (entry == -1)
            goto empty;
        PauseMenu_ClearSpriteFlags(p, 0x3B, 1);
        Anim_LoadKeyframe(&p->actionAnimation, p->motionResource, 0x18A, 1);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(p->actionDescriptionScript);
        MessageWindow_SetBounds(2, 16, 28, 2);
        MessageWindow_ShowMessage(entry);
        ScrollingDialogue_RequestShow();
        text = Script_ParseStringRef(p->actionDescriptionScript);
        text += 9;
        text += entry;
        ScrollingDialogue_SetScript(Text_LookupString(text));
    } else if (p->actionCursor <= 0x13) {
        entry = GameState_GetUnlockEntry(p->actionCursor - 8);
        if (!entry) {
        empty:
            p->actionAnimation.flags |= 1;
            MessageWindow_Clear();
            ScrollingDialogue_ClearScript();
        } else {
            PauseMenu_ClearSpriteFlags(p, 0x3B, 1);
            Anim_LoadKeyframe(&p->actionAnimation, p->motionResource, 0x18A, 1);
            MessageWindow_SetDisplayMode(1);
            MessageWindow_SetMessageTable(p->unlockTextScript);
            MessageWindow_SetBounds(2, 16, 28, 2);
            MessageWindow_ShowMessage(entry - 1);
            ScrollingDialogue_RequestShow();
            text = Script_ParseStringRef(p->unlockTextScript);
            text += 12;
            text += entry;
            ScrollingDialogue_SetScript(Text_LookupString(text));
        }
    }
}
