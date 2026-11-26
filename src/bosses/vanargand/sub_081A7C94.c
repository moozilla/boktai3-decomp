#include "global.h"

struct ScenePanelPairState {
    u8 pad0[0xAE];
    u8 mode;
    u8 padAF[0xE1 - 0xAF];
    u8 gauge;
    u8 padE2[0x56C - 0xE2];
    void *pair;
    u8 pad570[0x5C0 - 0x570];
    s16 counter;
    s16 variant;
    u8 state, cursor, transition, level;
};

extern const u8 gUnk_08611F74[];
s32 __divsi3(s32, s32);
s32 ScenePair_CountPositiveCounters(void *);
void ScenePanel_SelectPairTransition(void *, const u8 table[][3]);

s32 ScenePanel_UpdatePairConditions(struct ScenePanelPairState *p)
{
    struct ScenePanelPairState *q = p;
    s32 d = __divsi3(p->gauge, 43);
    if (3 - d != p->level) {
        p->level = 3 - d;
        p->state = 9;
        p->cursor = 0;
    } else if (p->counter > 127) {
        p->state = 10;
        p->cursor = 0;
        p->counter = 0;
    }
    if (p->variant == 1) {
        u32 m = q->mode;
        if (m == 11) {
            s32 n = ScenePair_CountPositiveCounters(p->pair);
            if (n == 0) {
                u8 s = p->state;
                if (s != 2 && s != 6 && s != 11 && s != 12 && s != 9) {
                    p->state = m;
                    p->cursor = n;
                }
            }
        }
    }
    if (p->variant == 0 && q->mode == 3)
        p->transition = 6;
    else
        ScenePanel_SelectPairTransition(p, (const u8(*)[3])gUnk_08611F74);
    return 1;
}
