#include "global.h"
struct MenuDisplay {
    u8 unknown0[0x4D80];
    s8 entries0[8];
    s8 entries1[8];
    s8 entries2[8];
    s8 entries3[4];
};
s32 MenuDisplay_FindEntryIndex(struct MenuDisplay *p, s32 entry, s32 group)
{
    s32 i;
    switch (group) {
        case 0:
            for (i = 0; i <= 7; i++)
                if (p->entries0[i] >= 0 && p->entries0[i] == entry)
                    return i;
            break;
        case 1:
            for (i = 0; i <= 7; i++)
                if (p->entries1[i] >= 0 && p->entries1[i] == entry)
                    return i;
            break;
        case 2:
            for (i = 0; i <= 7; i++)
                if (p->entries2[i] >= 0 && p->entries2[i] == entry)
                    return i;
            break;
        case 3:
            for (i = 0; i <= 3; i++)
                if (p->entries3[i] >= 0 && p->entries3[i] == entry)
                    return i;
            break;
    }
    return -1;
}
