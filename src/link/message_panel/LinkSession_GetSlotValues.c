#define gLinkDefaultSlotValues  gUnk_08606544
#define gLinkSelectedSlotValues gUnk_08606548
#include "global.h"
extern const u8 gUnk_08606544[], gUnk_08606548[];
void LinkSession_GetSlotValues(s32 selected, s32 *a, s32 *b, s32 *c, s32 *d)
{
    *a = gUnk_08606544[0];
    *b = gUnk_08606544[1];
    *c = gUnk_08606544[2];
    *d = gUnk_08606544[3];
    switch (selected) {
        case 2:
            *a = gUnk_08606548[0];
            break;
        case 0:
            *b = gUnk_08606548[1];
            break;
        case 1:
            *c = gUnk_08606548[2];
            break;
        case 3:
            *d = gUnk_08606548[3];
            break;
    }
}
