#include "global.h"
#include "scene_hud_display.h"
#include "scene_item_values.h"
#include "player.h"

u32 Presentation_SelectGraphicResource(void *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);

/* Equipment-description view of the scene HUD. The alternate entry is used in action mode six. */
struct SceneHudEquipmentPreview {
    u8 unk_00[0x40];
    const void *timedMessageScript;
    u8 unk_44[0xA4 - 0x44];
    u8 motionResource[0x20];
    u8 unk_C4[0xA40 - 0xC4];
    struct Player *actionOwner;
    u8 unk_A44[0x2A40 - 0xA44];
    struct SceneHudControlAnimation actionAnimation;
    u8 unk_2A60[0x4387 - 0x2A60];
    u8 actionCursor;
    u8 unk_4388[0x4884 - 0x4388];
    struct SceneValueEquipment alternateEquipment;
};

struct SceneHudDigitDisplay {
    u32 flags;
    u8 unk_04[0x18 - 4];
    struct ContactPosition position;
    u8 unk_20[0x60 - 0x20];
};

struct SceneHudEquipmentDescriptor {
    u32 words[8];
};
void Inventory_GetEquippedDescriptor(u32, struct SceneHudEquipmentDescriptor *);
s32 Player_GetEquipmentValue(const struct SceneHudEquipmentDescriptor *);
u16 *Hud_GetTilemapCell(s32, s32, s32);
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void MenuTilemap_ClearRect(s32, s32, s32, s32);
u32 Inventory_GetEquipmentId(u32);
void Inventory_GetEquipmentDescriptor(u32, struct SceneHudEquipmentDescriptor *);
s32 Div(s32, s32);

void SceneHud_RefreshEquipmentPreview(struct SceneHudEquipmentPreview *p)
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
    struct SceneHudEquipmentDescriptor selected;
    s32 digits[3];
    struct SceneHudEquipmentDescriptor equipped;
    s32 selectedValue;
    s32 remainder;
    s32 i, j;
    s32 x, y;
    s32 value;
    s32 difference;
    s32 changed;
    u16 *cell;
    struct SceneInputWidget *widget;
    struct SceneHudEquipmentDescriptor *equippedDescriptor;

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
                SceneHud_DrawDecimalDigits(Player_GetEquipmentValue(0), 100, tileX + 2, tileY, 0);
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
            SceneHud_DrawDecimalDigits(value, 100, x, y, 0);
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
                struct SceneHudDigitDisplay *display;
                MenuTilemap_ClearRect(x, y, 3, 1);
                position.x = x << 3;
                position.y = y << 3;
                resource = p->widgetResource;
                frameBase = difference + 10;
                digit = digits;
                display = (struct SceneHudDigitDisplay *)&p->motionWidgets[2].flags;
                widget = &p->motionWidgets[2];
                for (j = 0; j < 3; display++, widget++, j++) {
                    display->flags &= ~1;
                    display->position = position;
                    Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + frameBase), 1);
                    position.x += 8;
                }
            }
        }
    }
}
