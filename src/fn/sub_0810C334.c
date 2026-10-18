#include "global.h"
struct SlotMenu {
    u8 unknown0[0x260];
    u8 lastEntry[0x60];
    u8 lastEntryResource[0x40];
    u16 entryValues[7];
    u8 unknown30E[0xC];
    s16 cachedLastValue;
    u8 unknown31C[0x624];
    u8 mode;
};
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
void SlotMenu_UpdateLastEntry(struct SlotMenu *p)
{
    if (p->mode == 3)
        p->entryValues[6] = 0x8E;
    else
        p->entryValues[6] = 0x7F;
    if (p->entryValues[6] != p->cachedLastValue) {
        Anim_SelectWidgetGlyph(p->lastEntry, p->lastEntryResource, p->entryValues[6]);
        p->cachedLastValue = p->entryValues[6];
    }
}
