#define InventoryGrid_UpdateNumberPhase sub_0816DE28
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
    u8 unknown0A50[0x4CE0 - 0xA50];
    struct InventoryNumberDigit attachments[4];
};
s32 sub_08165D10(void *);
void sub_0816DC78(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816DE28(struct InventoryGrid *s)
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
        Hud_SetInteractionCallback(s, sub_0816DC78, 1);
    }
}
