#include "global.h"
#include "scene_hud_display.h"
#include "scene_item_values.h"
void sub_08173450(void *);
u32 Inventory_GetEquipmentId(u32);
void ActionRecord_CopyFromEntry(void *, void *);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
u32 Presentation_SelectGraphicResource(void *);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
void Presentation_ApplyGraphicData(void *, void *);
void ScrollingDialogue_RequestShow(void);
void Presentation_ApplyGraphicSettings(void *, const void *);
void SceneHud_ShowEquipmentDetails(struct SceneHudState *p)
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
        ActionRecord_CopyFromEntry(equipment, values);
        PauseMenu_ClearSpriteFlags(p, 0x3b, 1);
        anim = &p->actionAnimation;
        data = p->motionResource;
        Anim_LoadKeyframe(anim, data, (u16)Presentation_SelectGraphicResource(equipment), 1);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(p->timedMessageScript);
        MessageWindow_SetBounds(2, 0x10, 0x1c, 2);
        Presentation_ApplyGraphicData(values, p->unknownA44);
        ScrollingDialogue_RequestShow();
        Presentation_ApplyGraphicSettings(equipment, p->timedMessageScript);
    }
}
