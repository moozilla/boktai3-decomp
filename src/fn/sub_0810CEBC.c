#define SlotMenu_InitializeAlternateEntryValues sub_0810CEBC
#include "global.h"
struct SlotMenu {
    u8 unknown0[0x300];
    u16 entryValues[7];
};
void sub_0810CEBC(struct SlotMenu *p)
{
    p->entryValues[0] = 0x2D;
    p->entryValues[1] = 0x2E;
    p->entryValues[2] = 0x27;
    p->entryValues[3] = 0x28;
    p->entryValues[4] = 0x29;
    p->entryValues[5] = 0x2F;
    p->entryValues[6] = 0x2F;
}
