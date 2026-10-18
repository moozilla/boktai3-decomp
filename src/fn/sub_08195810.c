#include "global.h"

struct State08195810 {
    u8 pad0[0x7244];
    s16 timer;
    u8 pad7246[0x7255 - 0x7246];
    u8 dead[9];
    u8 pad725e[0x7278 - 0x725e];
    s16 hp[9];
};
s32 Nidhoggr_GetNextShuffledPart(void *);

void Nidhoggr_ActivateRandomPart(struct State08195810 *t)
{
    s32 tries;
    s32 i;
    if (t->timer <= 0x6d)
        return;
    t->timer = 0;
    tries = 9;
    for (;;) {
        i = Nidhoggr_GetNextShuffledPart(t);
        if (t->hp[i] > 0) {
            if (i <= 3) {
                if (!t->dead[i] && !t->dead[i + 4])
                    goto found;
            } else if (i <= 7) {
                if (!t->dead[i] && !t->dead[i - 4])
                    goto found;
            } else if (!t->dead[i])
                goto found;
        }
        if (--tries <= 0)
            return;
    }
found:
    t->dead[i] = 1;
}
