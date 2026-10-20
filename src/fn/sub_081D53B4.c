#include "global.h"

#define BestiaryFlags_Update sub_081D53B4

u32 GameState_AreAllEventFlagsSet(void);
void ThresholdTrack_ClassifyPosition(u8);
u32 Bestiary_IsGroupComplete(void);

void BestiaryFlags_Update(u8 *p)
{
    if (GameState_AreAllEventFlagsSet()) {
        u32 *q = (u32 *)(p + 0xC6C);
        q[2] &= ~1;
    } else {
        u32 *q = (u32 *)(p + 0xC6C);
        q[2] |= 1;
    }
    if (p[0x24] != 2) {
        if (p[0x24] == 0) {
            ThresholdTrack_ClassifyPosition(p[0x26]);
            if (Bestiary_IsGroupComplete()) {
                u32 *q = (u32 *)(p + 0xCCC);
                q[2] &= ~1;
            } else {
                u32 *q = (u32 *)(p + 0xCCC);
                q[2] |= 1;
            }
        } else {
            *(u32 *)(p + 0xCD4) |= 1;
        }
    }
}
