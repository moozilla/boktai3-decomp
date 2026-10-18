// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Panel {
    u8 pad0[0x32];
    u8 enabled;
    u8 pad33[9];
    u8 values[7];
};
struct Panel *WirelessExchange_Get(void);
void Sound_PlaySong(int);
int WirelessGame_ToggleItemMode(void)
{
    struct Panel *p = WirelessExchange_Get();
    asm("" : "+r"(p));
    if (p && p->enabled) {
        u8 *v = p->values;
        v[6] = 1 - v[6];
        Sound_PlaySong(267);
        return v[6];
    }
    return 0;
}
