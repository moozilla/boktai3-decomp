#include "global.h"
struct Panel {
    u8 pad0[0x3c];
    u8 counts[4];
};
struct Panel *WirelessExchange_Get(void);
void sub_0803A514(void *, int, int);
void WirelessGame_AddWeightedScore(int, int);
void Sound_PlaySong(int);
void WirelessGame_AddItem(void *p, int index)
{
    struct Panel *q = WirelessExchange_Get();
    if (q) {
        u8 *counts = q->counts;
        if (counts[index] <= 4) {
            int total = 0, i = 0;
            do {
                total += counts[i];
            } while (++i <= 3);
            if (total <= 9) {
                sub_0803A514(p, index, 1);
                counts[index]++;
                WirelessGame_AddWeightedScore(2, 1);
                Sound_PlaySong(885);
            }
        }
    }
}
