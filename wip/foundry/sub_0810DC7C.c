#include "global.h"
#define SelectionPanel_InitEntryPositions sub_0810DC7C
struct SelectionPanelEntry {
    u8 unk0[0x40];
    u16 x, y;
    u8 unk44[0x1c];
};
struct SelectionPanel {
    struct SelectionPanelEntry entries[10];
};
void sub_0810DC7C(struct SelectionPanel *p)
{
    s32 i = 0;
    struct SelectionPanelEntry *entry = (struct SelectionPanelEntry *)p;
    do {
        switch (i) {
            case 2:
                p->entries[2].x = 0;
                p->entries[2].y = -24;
                break;
            case 7:
                p->entries[7].x = 96;
                p->entries[7].y = 16;
                break;
            case 8:
                p->entries[8].x = 96;
                p->entries[8].y = 16;
                break;
            case 9:
                p->entries[9].x = 8;
                p->entries[9].y = 8;
                break;
            default:
                entry->x = 0;
                entry->y = 0;
                break;
        }
        entry++;
        i++;
    } while (i <= 9);
}
