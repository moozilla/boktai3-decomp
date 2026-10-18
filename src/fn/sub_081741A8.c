#define MotionSequence_ResolveAlternateSelection sub_081741A8
#define MotionSequence_SetCallback Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct MotionSequence { u8 unknown0[0x438A]; u8 selection; };
int sub_08165A30(void *);
void Sound_PlaySong(u32);
void PauseInventory_BeginItemDiscardSelection(void *, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
s32 MessageWindow_Clear(void);
void sub_08173DD4(void *, int, u32, int);
void Inventory_RemoveEquipmentEntry(u32);
void sub_0817373C(void *, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_UpdateInventoryDiscardAnimation(void);
void sub_081741A8(struct MotionSequence *p)
{
    int result = sub_08165A30(p);
    if (result == 0) {
        Sound_PlaySong(0xDE);
        PauseInventory_BeginItemDiscardSelection(p, 0);
    } else if (result == 1) {
        PauseMenu_AddSpriteFlags(p, 5, 1);
        MessageWindow_Clear();
        sub_08173DD4(p, 0, p->selection, 0x24);
        Inventory_RemoveEquipmentEntry(p->selection);
        sub_0817373C(p, 1);
        Hud_SetInteractionCallback(p, Hud_UpdateInventoryDiscardAnimation, 1);
    }
}
