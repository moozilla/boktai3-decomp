#include "global.h"
#include "area_effect_link.h"
#include "ram.h"
void LinkSelection_RecordSlotState(struct AreaEffectLinkState *p, u16 value, s32 slot, u32 *status,
                                   s32 skip)
{
    gSceneSelectionDisplaySlots[slot] = value & 0x3f3;
    if (!skip) {
        if (p->sendBit <= 5) {
            u32 bit = (value >> 2) & 1;
            p->selectionBits[slot + p->selectionPage * 4] |= bit << p->sendBit;
        }
        if (slot == 0) {
            *status = 1;
            if (value & 8)
                *status = 3;
        }
    }
}
