#include "global.h"
/* Provisional names and field interpretations; confidence low. */
#define Pool1BC_UpdateDelay sub_0810889C
struct NamedEntry {
    u8 pad0[0x60];
    u16 state;
    u8 pad62[3];
    u8 entered;
    u16 timer;
};
void sub_0810889C(void *unused, struct NamedEntry *entry)
{
    if (entry->entered) {
        entry->timer = 0;
        entry->entered = 0;
    }
    if (entry->timer > 10) {
        entry->state = 4;
        entry->entered = 1;
    }
    entry->timer++;
}
