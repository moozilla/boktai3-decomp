#define PanelLayout_ResetOffsets sub_0810CF90
#include "global.h"

struct PanelLayoutEntry {
    u8 pad0[0x40];
    s16 x, y;
    u8 pad44[0x1c];
};
struct PanelLayout {
    struct PanelLayoutEntry entries[7];
};
void sub_0810CF90(struct PanelLayout *layout)
{
    s32 i;
    for (i = 0; i <= 6; i++) {
        switch (i) {
        default:
            layout->entries[i].x = 0;
            layout->entries[i].y = 0;
                break;
        case 5:
            layout->entries[5].x = 0;
            layout->entries[5].y = 0;
            break;
        case 6:
            layout->entries[6].x = 0x90;
            layout->entries[i].y = 0;
            break;
        }
    }
}
