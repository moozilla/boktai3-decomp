#include "global.h"
void Storage_BeginItemSpecialMove(void *);
void Storage_BeginEquipmentSpecialMove(void *);
void sub_081BE730(void *);
void Storage_BeginSelectedItemTransition(u8 *p)
{
    u8 *a = p + 0x1754, *b = p + 0x1758;
    switch (b[*a] >> 1) {
        case 0:
            Storage_BeginItemSpecialMove(p);
            break;
        case 1:
            Storage_BeginEquipmentSpecialMove(p);
            break;
        case 2:
            sub_081BE730(p);
            break;
    }
}
