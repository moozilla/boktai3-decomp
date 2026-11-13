#include "global.h"
#include "scene_item_values.h"
#include "scene_hud_display.h"
#include "scene_inventory_panel.h"
#include "scene_vec.h"

extern const struct SceneValueItemDefinition gUnk_08E61280[];

u32 Inventory_GetItemStateValue(u32);

s32 Div(s32, s32);

s32 SceneInventory_GetItemConditionFrame(u32 slot)
{
    s32 item = Inventory_GetItemId(slot);
    u32 condition;
    s32 frame;
    if (item < 0 || (condition = gUnk_08E61280[item].conditionKey) == 0)
        return 394;
    frame = Div(Inventory_GetItemStateValue(slot) << 2, condition << 5);
    if (frame > 3)
        frame = 3;
    return frame + 395;
}

#define SceneHud_GetSelectedAction Inventory_GetQuantityByStorage

s32 Anim_LoadKeyframe(void *, void *, u32, u32);

s32 Script_ParseStringRef(const void *);

u8 *Text_LookupString(s32);

void ScrollingDialogue_RequestShow(void);

void PauseMenu_ClearSpriteFlags(struct SceneHudState *, s32, u32);

s32 Inventory_GetQuantityByStorage(u32, u32);

void SceneHud_ShowActionPreview(struct SceneHudState *s)
{
    s32 action, keyframe;
    s32 text;
    if (s->actionCursor <= 15) {
        action = Inventory_GetQuantityByStorage(s->actionCategory, s->actionCursor);
        if (s->actionCategory == 0)
            keyframe = SceneInventory_GetItemConditionFrame(s->actionCursor);
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

u32 PauseMenu_CanUseSelectedItem(void)
{
    return 1;
}

static inline void SetIconPosition(struct SceneVec *out, s32 x, s32 y)
{
    out->x = x;
    out->y = y;
    out->z = 0;
    out->pad = 0;
}

void SceneHud_GetActionIconPosition(struct SceneVec *out, s32 index)
{

    if (index <= 15) {
        s32 col = index & 3;
        s32 row = index >> 2;
        SetIconPosition(out, col * 24 + 0x70, row * 24 + 0x20);
        return;
    }
    if (index == 0x27)
        SetIconPosition(out, 0xD8, 0x50);
    else if (index == 0x26)
        SetIconPosition(out, 0xD8, 0x38);
    else if (index == 0x24 || index == 0x28)
        SetIconPosition(out, 0xD8, 0x68);
    else if (index == 0x20)
        SetIconPosition(out, 0xD8, 0x20);
    else {
        out->x = 0x90;
        out->y = 0x20;
    }
}

u32 Selection_GetDisplayFrame(s32);

s32 SceneHud_CanApplySelectedAction(s32);

void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);

void SceneHud_RefreshActionIcons(struct SceneHudState *owner)
{
    s32 i;
    u8 *category;
    u8 *base;
    u32 *flags;
    struct SceneInputWidget *w;
    i = 0;
    category = &owner->actionCategory;
    base = (u8 *)owner;
    flags = &owner->categoryWidgets[10].flags;
    w = &owner->categoryWidgets[10];
    for (; i < 16; base += sizeof(*w), w++, flags += sizeof(*w) / sizeof(*flags), i++) {
        s32 action = Inventory_GetQuantityByStorage(*category, i);
        if (action < 0) {
            *flags |= 1;
        } else {
            struct SceneInputWidget *current = ((struct SceneHudState *)base)->categoryWidgets + 10;
            current->flags &= ~1;
            Anim_LoadKeyframe(w, owner->motionResource, (u16)Selection_GetDisplayFrame(action), 1);
            if (*category == 0) {
                if ((u32)(action - 30) <= 1 || SceneHud_CanApplySelectedAction(action))
                    PauseMenu_SetSpritePalette(w, 0x373);
                else
                    PauseMenu_SetSpritePalette(w, 0x374);
            } else {
                PauseMenu_SetSpritePalette(w, 0x373);
            }
        }
    }
    owner = NULL;
}

#define SceneHud_FindCategoryAction Inventory_GetQuantityByStorage

#define SceneHud_SetIconPalette     PauseMenu_SetSpritePalette

s32 SceneHud_FindCategoryAction(u32, u32);

void SceneHud_SetIconPalette(struct SceneInputWidget *, u16);

void SceneHud_BeginActionIconMotion(struct SceneHudState *s, s32 index, s32 source, s32 target)
{
    struct SceneInputWidget *w = &s->motionWidgets[index];
    void *resource;
    w->flags &= ~1;
    resource = s->motionResource;
    Anim_LoadKeyframe(
        w, resource,
        (u16)Selection_GetDisplayFrame(SceneHud_FindCategoryAction(s->actionCategory, source)), 1);
    SceneHud_GetActionIconPosition(&w->position, source);
    SceneHud_GetActionIconPosition(&s->actionTargets[index], target);
    source += 10;
    s = (struct SceneHudState *)((u8 *)s + source * 0x60);
    s = (struct SceneHudState *)((u8 *)s + 0x145a);
    SceneHud_SetIconPalette(w, *(u16 *)s);
}

#define SceneHud_ClearWidgetFlags PauseMenu_ClearSpriteFlags

#define SceneHud_SetWidgetFlags   PauseMenu_AddSpriteFlags

void Hud_RestoreSecondaryGridSelection(void *);

void Inventory_InitializeSelectionGrid(void *, s32, s32);


void SceneHud_SetWidgetFlags(void *, s32, u32);

void InventoryPanel_ShowStatusIcons(void *);

void MenuDisplay_UpdateActionPrompt(void *, s32);

void MenuDisplay_DrawStatComparison(void *, s32, s32);

void MenuDisplay_SetPaletteOverrides(void *, s32, s32, s32, s32);

void Hud_SetInteractionCallback(void *, void (*)(void), s32);

void PauseMenu_UpdateItemActions(void);

void SceneHud_ReturnToActionGrid(struct SceneHudState *s)
{
    void *input;
    SceneHud_RefreshActionIcons(s);
    input = s->actionInput;
    Hud_RestoreSecondaryGridSelection(input);
    if (s->mode == 1)
        Inventory_InitializeSelectionGrid(input, 1, 0xe1);
    else
        Inventory_InitializeSelectionGrid(input, 1, 0x61);
    SceneHud_ClearWidgetFlags(s, 4, 1);
    SceneHud_SetWidgetFlags(s, 6, 1);
    SceneHud_SetWidgetFlags(s, 5, 1);
    SceneHud_GetActionIconPosition(&s->categoryWidgets[4].position, s->actionCursor);
    SceneHud_ShowActionPreview(s);
    InventoryPanel_ShowStatusIcons(s);
    MenuDisplay_UpdateActionPrompt(s, 4);
    MenuDisplay_DrawStatComparison(s, 10, 6);
    MenuDisplay_SetPaletteOverrides(s, 0, 1, 0, 0);
    Hud_SetInteractionCallback(s, PauseMenu_UpdateItemActions, 0);
}

void PauseMenu_BeginSpriteMotion(void *, u32, s32, u32, void *, void *, u32, u32);

void InventoryGrid_BeginNumberPhase(void);

void SceneHud_BeginSelectedActionTransfer(struct SceneHudState *s)
{
    struct ContactPosition from;
    struct SceneInputPackedPosition to;
    u8 *selection = &s->actionSelection;
    u32 index = *selection + 10;
    u32 offset = index * 0x60;
    u8 *base = (u8 *)s + 0x1428;
    u32 *flags = (u32 *)(base + offset);
    s32 keyframe;
    u32 later;
    *flags |= 1;
    SceneHud_GetActionIconPosition(&from, s->actionCursor);
    to.x = 16;
    to.y = 48;
    to.z = 0;
    keyframe =
        Selection_GetDisplayFrame(SceneHud_FindCategoryAction(s->actionCategory, *selection));
    later = *selection + 10;
    PauseMenu_BeginSpriteMotion(s, 0, keyframe, s->categoryWidgets[later].paletteBank, &from, &to, 24,
                             16);
    Sound_PlaySong(0x38a);
    Hud_SetInteractionCallback(s, InventoryGrid_BeginNumberPhase, 1);
}

#define SceneItem_SelectRandomSpecialItem Inventory_RollIcePopStick

#define SceneItem_SetSlotItem             Inventory_SetItemId

u32 SceneItem_SelectRandomSpecialItem(void);

void SceneItem_SetSlotItem(s32, s32);

void SceneHud_FinishRandomActionReturn(struct SceneHudState *);

void SceneHud_BeginRandomActionReturn(struct SceneHudState *s)
{
    struct SceneInputPackedPosition from;
    struct ContactPosition to;
    u8 *selection = &s->actionSelection;
    u32 index = *selection + 10;
    u32 offset = index * 0x60;
    u8 *base = (u8 *)s + 0x1428;
    u32 *flags = (u32 *)(base + offset);
    u32 item;
    u32 later;
    s32 keyframe;
    *flags |= 1;
    s->motionWidgets[0].flags &= ~1;
    from.x = 16;
    from.y = 48;
    from.z = 0;
    SceneHud_GetActionIconPosition(&to, s->actionCursor);
    item = SceneItem_SelectRandomSpecialItem();
    SceneItem_SetSlotItem(*selection, SceneItem_SelectRandomSpecialItem());
    keyframe = Selection_GetDisplayFrame(item);
    later = *selection + 10;
    PauseMenu_BeginSpriteMotion(s, 0, keyframe, s->categoryWidgets[later].paletteBank, &from, &to, 24,
                             16);
    Sound_PlaySong(0x38a);
    Hud_SetInteractionCallback(s, SceneHud_FinishRandomActionReturn, 1);
}

#define SceneItem_GetSlotItem   Inventory_GetItemId

#define SceneHud_TickIconMotion PauseMenu_UpdateSpriteMotion

u32 SceneHud_TickIconMotion(void *, u32);

s32 SceneItem_GetSlotItem(u32);

void SceneHud_UpdateActionSelection(void);

void Hud_SaveGridSelection(void *);

void InventoryPanel_DrawStatComparison(void *, s32, s32, s32, s32, s32);

void SceneHud_FinishRandomActionReturn(struct SceneHudState *s)
{
    void *input;
    if (SceneHud_TickIconMotion(s, 0)) {
        if (SceneItem_GetSlotItem(s->actionSelection) == 31)
            Sound_PlaySong(0x123);
        s->motionWidgets[0].flags |= 1;
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, SceneHud_UpdateActionSelection, 1);
        input = s->actionInput;
        Inventory_InitializeSelectionGrid(input, 1, 0);
        SceneHud_GetActionIconPosition(&s->categoryWidgets[5].position, s->actionCursor);
        SceneHud_ShowActionPreview(s);
        Hud_SaveGridSelection(input);
        InventoryPanel_ShowStatusIcons(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        InventoryPanel_DrawStatComparison(s, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
