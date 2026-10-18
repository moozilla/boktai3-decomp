#define SceneHud_ShowEquipmentDetails sub_08173470
#include "global.h"
#include "scene_hud_display.h"
#include "scene_item_values.h"
void sub_08173450(void *);
u32 Inventory_GetEquipmentId(u32);
void sub_0822D764(void *, void *);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
u32 sub_08174594(void *);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
void sub_08174480(void *, void *);
void ScrollingDialogue_RequestShow(void);
void sub_081744AC(void *, const void *);
void sub_08173470(struct SceneHudState *p)
{
    u8 values[0x20];
    u8 *cursor = &p->actionCursor;
    struct SceneValueEquipment *equipment;
    void *anim;
    void *data;
    if (*cursor > 15 || Inventory_GetEquipmentId(*cursor) == 0xff) {
        sub_08173450(p);
    } else {
        equipment = Inventory_GetEquipmentEntry(*cursor);
        sub_0822D764(equipment, values);
        PauseMenu_ClearSpriteFlags(p, 0x3b, 1);
        anim = &p->actionAnimation;
        data = p->motionResource;
        Anim_LoadKeyframe(anim, data, (u16)sub_08174594(equipment), 1);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(p->timedMessageScript);
        MessageWindow_SetBounds(2, 0x10, 0x1c, 2);
        sub_08174480(values, p->unknownA44);
        ScrollingDialogue_RequestShow();
        sub_081744AC(equipment, p->timedMessageScript);
    }
}
