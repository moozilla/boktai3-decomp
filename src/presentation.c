#include "presentation.h"

void TextLabel_FormatSuffix(struct TextLabelStyle *style, u8 *text)
{
    u32 length = 0;
    switch (style->suffixKind) {
        case 1:
            text[0] = 0x80;
            text[1] = 0xCA;
            length = 2;
            break;
        case 2:
            text[0] = 0x80;
            text[1] = 0xCB;
            length = 2;
            break;
    }
    if (style->specialC || style->special10) {
        text[length] = 'S';
        text[length + 1] = 'P';
        length += 2;
    }
    text[length] = 0;
}

s32 TextWindow_Refresh(s32, s32);

void TextWindow_SetString(int, int, void *);

s32 MessageWindow_ShowMessage(s32);

int MessageWindow_SetStringArgument(int, void *);

void TextLabel_DisplaySuffix(s32 useWindow, s32 id, struct TextLabelStyle *style, u8 *text)
{
    TextLabel_FormatSuffix(style, text);
    if (useWindow) {
        TextWindow_Refresh(id, style->index);
        TextWindow_SetString(id, 0, text);
    } else {
        MessageWindow_ShowMessage(style->index);
        MessageWindow_SetStringArgument(0, text);
    }
}

void Presentation_ApplyGraphicData(struct TextLabelStyle *data, u8 *target)
{
    TextLabel_DisplaySuffix(0, -1, data, target);
}

void Presentation_ApplyIndexedGraphicData(s32 index, struct TextLabelStyle *data, u8 *target)
{
    TextLabel_DisplaySuffix(1, index, data, target);
}

s32 Script_ParseStringRef(const void *);

void ScrollingDialogue_SetNumberArgument(u32, u32);

void ScrollingDialogue_SetScript(const void *);

void ScrollingDialogue_SetStringArgument(u32, const void *);

void Presentation_ApplyGraphicSettings(struct SceneValueEquipment *req, const void *script)
{
    const struct PresentationEquipmentDefinition *row = &gUnk_08E6092C[req->kind];
    u8 *base = (u8 *)Script_ParseStringRef(script);
    u8 *strings = base + 0x31;
    s32 i;
    if (row->requiredLevel)
        ScrollingDialogue_SetNumberArgument(0, row->requiredLevel);
    ScrollingDialogue_SetNumberArgument(1, row->numberB);
    ScrollingDialogue_SetScript(Text_LookupString(strings + row->stringId));
    i = 0;
    if (req->modifierA)
        ScrollingDialogue_SetStringArgument(0, Text_LookupString(strings + (row->stringB + 0x31)));
    else
        ScrollingDialogue_SetStringArgument(0, Text_LookupString(base + 0x62));
    i++;
    if (req->modifierB)
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + (row->stringC + 0x31)));
    else
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + 0x31));
    i++;
    if (req->condition == 0) {
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + 0x49));
        i++;
    }
    while (i <= 3) {
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + 0x31));
        i++;
    }
}

void Presentation_ApplyPackedGraphicData(const void *data, const void *target)
{
    struct SceneValueEquipment settings;
    Inventory_InitEquipmentEntryFromDescriptor(data, &settings);
    Presentation_ApplyGraphicSettings(&settings, target);
}

u32 Presentation_SelectGraphicResource(struct SceneValueEquipment *data)
{
    if (data->condition == 0 || data->kind == 0x30 || data->kind == 0x2f)
        return 394;
    if (data->condition <= 250)
        return 398;
    if (data->condition <= 500)
        return 397;
    if (data->condition <= 750)
        return 396;
    else
        return 395;
}

void MessageWindow_Clear(void);

void ScrollingDialogue_ClearScript(void);

s32 MessageWindow_SetDisplayMode(s32);

s32 MessageWindow_SetMessageTable(const void *);

s32 MessageWindow_SetBounds(s32, s32, s32, s32);

void Inventory_GetEquippedDescriptor(u32, struct SceneInputItemDescriptor *);

s32 Player_GetEquipmentValue(const struct SceneInputItemDescriptor *);

u16 *Hud_GetTilemapCell(s32, s32, s32);

void Hud_DrawDecimalDigits(s32, s32, s32, s32, s32);

void PauseMenu_ClearTileRect(s32, s32, s32, s32);

u32 Inventory_GetEquipmentId(u32);

void Inventory_GetEquipmentDescriptor(u32, struct SceneInputItemDescriptor *);

s32 Div(s32, s32);

void SceneHud_RefreshEquipmentPreview(struct SceneHudState *p)
{
    struct SceneValueEquipment *equipment;
    if (p->actionCursor <= 15) {
        if (p->actionOwner->actionMode == 6)
            equipment = &p->alternateEquipment;
        else
            equipment = Inventory_GetEquipmentEntry(p->actionCursor);
        if (equipment->kind != 255)
            goto show;
    }
    p->actionAnimation.flags |= 1;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    return;
show:
    p->actionAnimation.flags &= ~1;
    {
        void *anim = &p->actionAnimation;
        void *data = p->motionResource;
        Anim_LoadKeyframe(anim, data, (u16)Presentation_SelectGraphicResource(equipment), 1);
    }
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetMessageTable(p->timedMessageScript);
    MessageWindow_SetBounds(2, 16, 28, 2);
}

/* Tentative: show equipped values and highlight a changed weapon value with keyframe digits. */

void SceneHud_RefreshEquipmentValues(struct SceneHudState *p)
{
    struct ContactPosition position;
    struct SceneInputItemDescriptor selected;
    s32 digits[3];
    struct SceneInputItemDescriptor equipped;
    s32 selectedValue;
    s32 remainder;
    s32 i, j;
    s32 x, y;
    s32 value;
    s32 difference;
    s32 changed;
    u16 *cell;
    struct SceneInputWidget *widget;
    struct SceneInputItemDescriptor *equippedDescriptor;

    /* categoryWidgets[62 + i] and motionWidgets[2 + i] are overlapping HUD views. */
    for (i = 0; i < 6; i++)
        p->categoryWidgets[i + 62].flags |= 1;
    if (p->actionOwner->actionMode == 6) {
        s32 tileX, tileY;
        for (i = 0; i < 4; i++) {
            tileX = (i & 1) * 6 + 5;
            tileY = (i >> 1) + 5;
            cell = Hud_GetTilemapCell(0, tileX, tileY);
            if (i <= 2) {
                for (j = 0; j < 5; j++)
                    *cell++ = 0xF001;
            } else {
                *cell = 0xF071;
                Hud_DrawDecimalDigits(Player_GetEquipmentValue(0), 100, tileX + 2, tileY, 0);
            }
        }
    } else if (p->actionCursor > 15) {
        for (i = 0; i < 4; i++)
            Inventory_GetEquippedDescriptor(i, &selected);
    } else if (Inventory_GetEquipmentId(p->actionCursor) == 255) {
        for (i = 0; i < 4; i++)
            Inventory_GetEquippedDescriptor(i, &selected);
    } else {
        Inventory_GetEquipmentDescriptor(p->actionCursor, &selected);
        selectedValue = Player_GetEquipmentValue(&selected);
        digits[0] = Div(selectedValue, 100);
        remainder = selectedValue - digits[0] * 100;
        digits[1] = Div(remainder, 10);
        digits[2] = remainder - digits[1] * 10;
        for (i = 0, equippedDescriptor = &equipped; i < 4; i++) {
            Inventory_GetEquippedDescriptor(i, equippedDescriptor);
            value = Player_GetEquipmentValue(equippedDescriptor);
            x = (i & 1) * 6 + 7;
            y = (i >> 1) + 5;
            Hud_DrawDecimalDigits(value, 100, x, y, 0);
            if (i == 3)
                Hud_GetTilemapCell(0, 11, 6);
            changed = 0;
            difference = 0;
            if (i == 0 && selectedValue != value) {
                changed = 1;
                difference = 10;
                if (selectedValue > value)
                    difference = 0;
            }
            if (changed) {
                void *resource;
                s32 frameBase;
                s32 *digit;
                u32 *display;
                PauseMenu_ClearTileRect(x, y, 3, 1);
                position.x = x << 3;
                position.y = y << 3;
                resource = p->widgetResource;
                frameBase = difference + 10;
                digit = digits;
                display = &p->motionWidgets[2].flags;
                widget = &p->motionWidgets[2];
                for (j = 0; j < 3; display += sizeof(*widget) / sizeof(*display), widget++, j++) {
                    *display &= ~1;
                    ((struct SceneInputWidget *)((u8 *)display - (u32) &
                                                 ((struct SceneInputWidget *)0)->flags))
                        ->position = position;
                    Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + frameBase), 1);
                    position.x += 8;
                }
            }
        }
    }
}

void InventoryGrid_GetSlotPosition(void *output, s32 slot)
{
    struct InventoryGridPosition *p = output;
    if (slot <= 15) {
        p->x = 0x90 + (slot & 3) * 24;
        p->y = 0x20 + (slot >> 2) * 24;
    } else if (slot == 39) {
        p->x = 216;
        p->y = 80;
    } else if (slot == 36) {
        p->x = 216;
        p->y = 104;
    } else if (slot == 32) {
        p->x = 64;
        p->y = 76;
    } else if (slot == 33) {
        p->x = 88;
        p->y = 76;
    } else if (slot == 34) {
        p->x = 64;
        p->y = 104;
    } else if (slot == 35) {
        p->x = 88;
        p->y = 104;
    } else {
        p->x = 144;
        p->y = 32;
    }
}

void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);

void InventoryGrid_GetSlotPosition(void *, s32);

s32 sub_0822D730(s32);

/* Tentative: refresh equipment icons, with a fixed icon in alternate action mode. */

void SceneHud_RefreshEquipmentAvailability(struct SceneHudState *owner)
{
    s32 i;
    u32 *flags;
    u32 mask;

    if (owner->actionOwner->actionMode == 6) {
        struct SceneInputWidget *widget;
        owner->categoryWidgets[10].flags &= ~1;
        widget = &owner->categoryWidgets[10];
        Anim_LoadKeyframe(widget, owner->motionResource, 0x2F, 1);
        PauseMenu_SetSpritePalette(widget, 0x373);
        mask = 1;
        flags = &owner->categoryWidgets[11].flags;
        i = 14;
        do {
            *flags |= mask;
            flags += sizeof(*widget) / sizeof(*flags);
        } while (--i >= 0);
    } else {
        struct SceneInputWidget *widget;
        i = 0;
        widget = &owner->categoryWidgets[10];
        do {
            s32 id = Inventory_GetEquipmentId(i);
            if (id == 255) {
                owner->categoryWidgets[i + 10].flags |= 1;
            } else {
                owner->categoryWidgets[i + 10].flags &= ~1;
                Anim_LoadKeyframe(widget, owner->motionResource, (u16)id, 1);
                PauseMenu_SetSpritePalette(widget, 0x373);
                if (gGameState->level >= gUnk_08E6092C[id].requiredLevel)
                    PauseMenu_SetSpritePalette(widget, 0x373);
                else
                    PauseMenu_SetSpritePalette(widget, 0x374);
            }
            widget++;
            i++;
        } while (i <= 15);
    }
}

/* Tentative: configure one icon transfer between equipment-grid positions. */

void SceneHud_ConfigureEquipmentTransfer(struct SceneHudState *owner, s32 index, s32 from, s32 to)
{
    struct SceneInputWidget *widget = &owner->motionWidgets[index];
    void *motion;
    widget->flags &= ~1;
    motion = owner->motionResource;
    Anim_LoadKeyframe(widget, motion, (u16)Inventory_GetEquipmentId(from), 1);
    InventoryGrid_GetSlotPosition(&widget->position, from);
    InventoryGrid_GetSlotPosition(&owner->actionTargets[index], to);
    PauseMenu_SetSpritePalette(widget, owner->categoryWidgets[from + 10].paletteBank);
}

/* Tentative: availability bits for an equipment slot's action category. */

u32 SceneHud_GetEquipmentActionFlags(void *unused, s32 slot)
{
    u32 flags = 0;
    s32 category = sub_0822D730(slot);
    s32 id;
    u32 available;
    u32 offset;
    const struct PresentationEquipmentDefinition *table;
    const struct PresentationEquipmentDefinition *initialTable;
    u32 initialOffset;
    struct GameState **state;
    if (category == 3)
        return 0;
    id = Inventory_GetEquipmentId(slot);
    available = 1;
    initialTable = gUnk_08E6092C;
    initialOffset = id * sizeof(*initialTable);
    if (((const struct PresentationEquipmentDefinition *)((const u8 *)initialTable + initialOffset))
            ->group == 0) {
        table = initialTable;
        offset = initialOffset;
        state = &gGameState;
    } else {
        s16 *equipped;
        s32 remaining;
        table = initialTable;
        offset = initialOffset;
        state = &gGameState;
        equipped = (*state)->inventory;
        remaining = 3;
        do {
            if (*equipped == slot)
                available = 0;
            equipped++;
        } while (--remaining >= 0);
        if (available)
            flags |= 0x10;
    }
    if ((*state)->level >=
        ((const struct PresentationEquipmentDefinition *)(offset + (u32)table))->requiredLevel) {
        if (category == 0)
            flags |= 1;
        else if (category == 1)
            flags |= 2;
        else if (category == 2)
            flags |= 4;
        else
            flags |= 8;
    }
    return flags;
}

void MenuDisplay_DrawStatComparison(void *, s32, s32);

void Presentation_DrawEntryCount(struct SceneHudState *panel)
{
    Hud_DrawDecimalDigits(panel->entryCount, 100, 14, 12, 0);
    MenuDisplay_DrawStatComparison(panel, 5, 13);
}

void Presentation_SetAmountGlyphRows(struct SceneHudState *s)
{
    s32 i;
    for (i = 0; i < 3; i++) {
        s->motionWidgets[2 + i * 3].flags &= ~1;
        s->motionWidgets[2 + i * 3].position.x = 40;
        s->motionWidgets[2 + i * 3].position.y = 104 + i * 8;
        s->motionWidgets[3 + i * 3].flags &= ~1;
        s->motionWidgets[3 + i * 3].position.x = 48;
        s->motionWidgets[3 + i * 3].position.y = 104 + i * 8;
        s->motionWidgets[4 + i * 3].flags &= ~1;
        s->motionWidgets[4 + i * 3].position.x = 56;
        s->motionWidgets[4 + i * 3].position.y = 104 + i * 8;
    }
    Presentation_DrawEntryCount(s);
}

void Hud_DrawHealthMeterCells(void *, u32, u32, u32, u32, u32, u32);

void Hud_DrawEnergyMeter(void *, s32, s32, s32, s32, s32, s32);

void InventoryPanel_UpdateSelectedUnlock(void *, s32, s32, s32, s32);

void InventoryPanel_UpdateSelectedEquipment(void *, s32, s32, s32, s32);

void InventoryGrid_UpdateStatPreview(struct SceneHudState *p)
{
    struct SceneInputItemDescriptor descriptor;
    s32 t[2];
    s32 value;
    value = p->comparisonValues[0] + p->comparisonOffsets[0] + p->comparisonDeltas[0];
    if (value > 120)
        value = 120;
    t[0] = value * 10;
    Hud_DrawHealthMeterCells(p, 1, 8, 8, 1, p->actionOwner->currentStatA, t[0]);
    value = p->comparisonValues[1] + p->comparisonOffsets[1] + p->comparisonDeltas[1];
    if (value > 120)
        value = 120;
    t[0] = value * 5 + 100;
    Hud_DrawEnergyMeter(p, 1, 8, 10, p->actionOwner->currentStatB, t[0], 1);
    Inventory_GetEquippedDescriptor((s16)gGameState->unk50, &descriptor);
    t[1] = 7;
    InventoryPanel_UpdateSelectedUnlock(p, 20, t[1], 0, p->comparisonOffsets[2]);
    InventoryPanel_UpdateSelectedEquipment(p, 26, 7, 0, p->comparisonOffsets[2]);
}

#define STAT_MARKER_OFFSET(row)                                                                    \
    ((u32) & ((struct SceneHudState *)0)->categoryWidgets[85 + (row) * 3])

void InventoryGrid_LayoutStatMarkers(struct SceneHudState *p)
{
    s32 i;
    for (i = 0; i < 3; i++) {
        u32 address = (u32)p + i * sizeof(struct SceneInputWidget);
        struct SceneInputWidget *w = (void *)(address + STAT_MARKER_OFFSET(0));
        w->flags &= ~1;
        p->categoryWidgets[i + 85].position.x = p->comparisonValues[i] - 100;
        p->categoryWidgets[i + 85].position.y = i * 8;
    }
    for (i = 0; i < 3; i++) {
        u32 address = (u32)p + i * sizeof(struct SceneInputWidget);
        struct SceneInputWidget *w = (void *)(address + STAT_MARKER_OFFSET(1));
        w->flags &= ~1;
        p->categoryWidgets[i + 88].position.x =
            p->categoryWidgets[i + 85].position.x + p->comparisonOffsets[i];
        p->categoryWidgets[i + 88].position.y = i * 8;
    }
    for (i = 0; i < 3; i++) {
        u32 address = (u32)p + i * sizeof(struct SceneInputWidget);
        struct SceneInputWidget *w = (void *)(address + STAT_MARKER_OFFSET(2));
        w->flags &= ~1;
        p->categoryWidgets[i + 91].position.x =
            p->categoryWidgets[i + 88].position.x + p->comparisonDeltas[i];
        p->categoryWidgets[i + 91].position.y = i * 8;
    }
}

void Presentation_UpdateListCursor(struct SceneHudState *panel)
{
    panel->categoryWidgets[5].position.x = 0;
    panel->categoryWidgets[5].position.y = panel->rowIndex * 8;
}

void Presentation_CommitListValues(struct SceneHudState *panel)
{
    s32 i;
    for (i = 0; i <= 2; i++) {
        *(u16 *)((u8 *)gGameState + i * sizeof(u16) + (u32) & ((struct GameState *)0)->baseStats) +=
            panel->comparisonOffsets[i];
        panel->comparisonOffsets[i] = 0;
    }
    *(u16 *)gGameState->unk42 = panel->entryCount;
}

void Presentation_InitListValues(struct SceneHudState *panel)
{
    s32 i;
    if (panel->actionOwner->actionMode != 6) {
        if (*(s16 *)gGameState->unk42 > 196)
            *(s16 *)gGameState->unk42 = 0;
        panel->entryCount = *(u16 *)gGameState->unk42;
    } else {
        panel->entryCount = 0;
    }
    {
        u16 zero = 0;
        for (i = 2; i >= 0; i--)
            panel->comparisonOffsets[i] = zero;
    }
}
