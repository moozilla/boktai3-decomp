#include "global.h"
struct Panel {
    u8 pad0[0xdc];
    int total;
    u8 pade0[2];
    u16 weights[4];
};
struct Panel *WirelessExchange_Get(void);
void WirelessGame_AddWeightedScore(int index, int amount)
{
    struct Panel *p = WirelessExchange_Get();
    if (p) {
        int *total = &p->total;
        int weight = p->weights[index];
        {
            int value = weight;
            value *= amount;
            *total += value;
        }
    }
}
