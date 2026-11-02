#include "date_entry_menu.h"

extern const struct ContactPosition gUnk_0824FCCC[12];
s32 Widget_InitLinked(u8 *, void *, u16, u32, u8, u8, u8, const struct ContactPosition *);

void sub_081DE630(struct DateMenu *menu)
{
    struct ContactPosition pos[12];
    s32 i;
    memcpy(pos, gUnk_0824FCCC, sizeof(pos));
    for (i = 0; i < 12; i++) {
        Widget_InitLinked((u8 *)&menu->digits[i], menu->widgets[0].resource, 0, 0x30, 0, 0, 0,
                          &pos[i]);
        menu->digits[i].flags |= 1;
    }
    menu->year = 0;
    menu->month = 0;
    menu->day = 0;
    menu->hour = 0;
    menu->minute = 0;
}
