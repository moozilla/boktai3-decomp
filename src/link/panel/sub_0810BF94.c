#include "global.h"
struct SlotMenu {
    u8 unknown0[0x300];
    u16 entryValues[7];
};
void SlotMenu_InitializeEntryValues(struct SlotMenu *p)
{
    p->entryValues[0] = 0x2C;
    p->entryValues[1] = 0x2C;
    p->entryValues[2] = 0x2D;
    p->entryValues[3] = 0x2E;
    p->entryValues[4] = 0x27;
    p->entryValues[5] = 0x28;
    p->entryValues[6] = 0x7F;
}
