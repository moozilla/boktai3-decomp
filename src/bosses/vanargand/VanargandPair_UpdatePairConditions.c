#include "global.h"
#include "bosses/vanargand/vanargand_pair_state_layout.h"

extern const u8 gUnk_08611F74[];
s32 __divsi3(s32, s32);
s32 ScenePair_CountPositiveCounters(void *);
void VanargandPair_SelectPairTransition(void *, const u8 table[][3]);

s32 VanargandPair_UpdatePairConditions(struct VanargandPairState *p)
{
    struct VanargandPairState *q = p;
    s32 quotient = __divsi3(p->gauge, 43);
    if (3 - quotient != p->level) {
        p->level = 3 - quotient;
        p->state = 9;
        p->cursor = 0;
    } else if (p->counter > 127) {
        p->state = 10;
        p->cursor = 0;
        p->counter = 0;
    }
    if (p->variant == 1) {
        u32 mode = q->mode;

        if (mode == 11) {
            s32 count = ScenePair_CountPositiveCounters(p->pair);

            if (count == 0) {
                u8 state = p->state;

                if (state != 2 && state != 6 && state != 11 && state != 12 && state != 9) {
                    p->state = mode;
                    p->cursor = count;
                }
            }
        }
    }
    if (p->variant == 0 && q->mode == 3)
        p->transition = 6;
    else
        VanargandPair_SelectPairTransition(p, (const u8(*)[3])gUnk_08611F74);
    return 1;
}
