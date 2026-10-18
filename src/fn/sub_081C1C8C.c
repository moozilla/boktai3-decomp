#include "global.h"
#include "player.h"
#define SegmentDisplay_UpdatePlayerSegments sub_081C1C8C
#define gPlayerSlots                        gUnk_02000580
struct SegmentDisplayElement {
    u8 unknown0[0x10];
    u16 frame;
    u8 unknown12[0x1a];
};
struct SegmentDisplay {
    u8 unknown0[0x100];
    struct SegmentDisplayElement segments[7];
};
void sub_081C1C8C(struct SegmentDisplay *p)
{
    s32 count = gPlayerSlots[0]->displaySegmentCount;
    s32 i;
    if (count > 7)
        count = 7;
    i = 0;
    if (i < count) {
        u16 one = 1, two = 2;
        do {
            if (i <= 3)
                ((struct SegmentDisplayElement *)((u8 *)p + 0x100 + i * 0x2c))->frame = one;
            else
                ((struct SegmentDisplayElement *)((u8 *)p + 0x100 + i * 0x2c))->frame = two;
            i++;
        } while (i < count);
    }
    for (; i < 7; i++)
        ((struct SegmentDisplayElement *)((u8 *)p + 0x100 + i * 0x2c))->frame = 0;
}
