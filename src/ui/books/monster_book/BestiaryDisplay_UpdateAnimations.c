#include "global.h"
void DisplayAnimation_UpdatePlayback(u8 *);
struct BestiaryAnimations {
    u8 pad0[0x28];
    u8 entries[5][0x94];
    u8 badge[0x94];
};
void BestiaryDisplay_UpdateAnimations(struct BestiaryAnimations *p)
{
    s32 i;

    for (i = 0; i < 5; i++)
        DisplayAnimation_UpdatePlayback(p->entries[i]);
    DisplayAnimation_UpdatePlayback(p->badge);
}
