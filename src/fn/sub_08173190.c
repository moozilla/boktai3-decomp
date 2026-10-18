#define MotionSequence_ConfirmReset sub_08173190
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct MotionSequence { u8 unknown0[0x4388]; u8 selectionA, selectionB, selection; };
int sub_08165A30(void *);
void Sound_PlaySong(u32);
void PauseInventory_ReturnToEquipmentSelection(void *);
void sub_0822E86C(u32);
void SolarFruit_SortInventorySlots(void *);
void sub_08173190(struct MotionSequence *p)
{
    s32 r = sub_08165A30(p);
    if (r == 0) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToEquipmentSelection(p);
    } else if (r == 1) {
        Sound_PlaySong(0xDD);
        sub_0822E86C(0);
        SolarFruit_SortInventorySlots(0);
        p->selectionA = 0;
        p->selectionB = 0;
        p->selection = 0;
        PauseInventory_ReturnToEquipmentSelection(p);
    }
}
