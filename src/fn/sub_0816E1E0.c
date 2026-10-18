#define InventoryGrid_GetSlotPosition   sub_0816D920
#define InventoryGrid_FinishNumberPhase sub_0816E1E0
#define InventoryGrid_SetPhaseCallback Hud_SetInteractionCallback
#include "global.h"

struct InventoryNumberDigit {
    u32 flags;
    u8 unknown04[0x14];
    struct InventoryNumberPosition {
        s16 x, y, z, pad;
    } position;
    u8 unknown20[8];
};
struct InventoryGrid {
    u8 unknown0000[0xA4E];
    u16 phaseCounter;
    u8 unknown0A50[0x1620 - 0xA50];
    s16 slotPosition[4];
    u8 unknown1628[0x4384 - 0x1628];
    u8 selectionState[3];
    u8 selectedSlot;
    u8 unknown4388[0x4CE0 - 0x4388];
    struct InventoryNumberDigit attachments[4];
};
s32 sub_08165D10(void *);
void sub_0816FF4C(void);
void sub_081663A0(void *, int, int, int, int);
void sub_08166B7C(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081672FC(void *);
void Hud_SaveGridSelection(void *);
void sub_0816CDB8(void *, int, int);
void sub_0816D868(void *);
void sub_0816D920(void *, int);
void sub_0816D98C(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816E1E0(struct InventoryGrid *s)
{
    if (s->phaseCounter <= 31) {
        if (++s->phaseCounter == 32) {
            u32 mask = 1;
            struct InventoryNumberDigit *p = s->attachments;
            s32 i = 3;
            do {
                p->flags |= mask;
                p++;
            } while (--i >= 0);
        } else if (s->phaseCounter <= 8) {
            u16 *p = &s->attachments[0].position.y;
            s32 i = 3;
            do {
                (*p)--;
                p += 0x14;
            } while (--i >= 0);
        }
    } else if (sub_08165D10(s)) {
        void *selection;
        sub_0816D98C(s);
        Hud_SetInteractionCallback(s, sub_0816FF4C, 1);
        selection = s->selectionState;
        sub_0816CDB8(selection, 1, 0);
        sub_0816D920(s->slotPosition, s->selectedSlot);
        sub_0816D868(s);
        Hud_SaveGridSelection(selection);
        sub_081672FC(s);
        sub_08166B7C(s, 4);
        sub_0816701C(s, 0, 0, 0, 1, 0);
        sub_081663A0(s, 1, 0, 0, 0);
    }
}
