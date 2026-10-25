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

#ifdef NONMATCHING
// fuzzy: 37.3%
// scope shape 258 (differences once low registers are set aside)
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
#else
INCLUDE_ASM("asm/nonmatching", sub_081745DC);
#endif
