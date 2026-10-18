#define MotionSequence_SetCallback Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct MotionSequence {
    u8 unknown0[0x438A];
    u8 selection;
};
int SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(u32);
void PauseInventory_BeginItemDiscardSelection(void *, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
s32 MessageWindow_Clear(void);
void SceneHud_InitEquipmentTransfer(void *, int, u32, int);
void Inventory_RemoveEquipmentEntry(u32);
void SceneHud_RefreshEquipmentIcons(void *, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_UpdateInventoryDiscardAnimation(void);
void MotionSequence_ResolveAlternateSelection(struct MotionSequence *p)
{
    int result = SceneHud_UpdateBinaryChoice(p);
    if (result == 0) {
        Sound_PlaySong(0xDE);
        PauseInventory_BeginItemDiscardSelection(p, 0);
    } else if (result == 1) {
        PauseMenu_AddSpriteFlags(p, 5, 1);
        MessageWindow_Clear();
        SceneHud_InitEquipmentTransfer(p, 0, p->selection, 0x24);
        Inventory_RemoveEquipmentEntry(p->selection);
        SceneHud_RefreshEquipmentIcons(p, 1);
        Hud_SetInteractionCallback(p, Hud_UpdateInventoryDiscardAnimation, 1);
    }
}
