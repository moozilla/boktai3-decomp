#include "global.h"

// MATCHING_CONCESSION: register-pinning

/* Signed view of the history entries read by this selector. */
struct PlayerHistory {
    u8 unknown0[0x34C];
    s16 directionHistory[10];
    u8 unknown360[0x3A4 - 0x360];
    u8 facing;
};

s32 Player_SelectHistoryDirection(struct PlayerHistory *arg)
{
    struct PlayerHistory *p = arg;
    s32 direction = p->directionHistory[1];
    register s32 initialMissing asm("r1") = -1;
    if (direction != initialMissing) {
        s32 next, previous, i;
        register s32 missing asm("r6");
        const s16 *history;
        if (direction & 1)
            return direction;
        next = (direction + 1) & 7;
        previous = (direction + 7) & 7;
        i = 2;
        missing = initialMissing;
        history = &p->directionHistory[2];
        do {
            s32 sample = *history;
            if (sample != missing) {
                if (sample == next || sample == previous)
                    return sample;
                if (sample != direction)
                    return direction;
            }
            history++;
        } while (++i <= 9);
    }
    return p->facing;
}
