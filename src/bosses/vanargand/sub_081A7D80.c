#include "global.h"

struct Transition {
    u8 pad0[0xAE];
    u8 mode;
    u8 active;
    u8 pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
    u8 padC8[0x56C - 0xC8];
    void *pair;
    u8 pad570[0x5C2 - 0x570];
    s16 condition;
    u8 pad5C4[2];
    u8 action;
    u8 variant;
};
s32 ScenePair_CountPositiveCounters(void *);
static inline void Request(struct Transition *p, u32 mode)
{
    p->mode = mode;
    p->counter = 0;
    p->pending = 1;
}
void ScenePanel_ApplyPairTransition(struct Transition *p)
{
    struct Transition *q = p;
    switch (q->action - 1) {
        case 0:
            if (ScenePair_CountPositiveCounters(q->pair) == 2) {
                if (q->variant == 3) {
                    Request(p, 6);
                    break;
                }
                if (q->variant > 1) {
                    Request(p, 5);
                    break;
                }
            }
            Request(p, 4);
            break;
        case 1:
            if (q->condition == 0) {
                Request(p, 12);
                return;
            }
            Request(p, 3);
            break;
        case 2:
            if (q->variant > 1)
                Request(p, 8);
            else
                Request(p, 7);
            break;
        case 3:
            Request(p, 9);
            break;
        case 4:
            if (ScenePair_CountPositiveCounters(q->pair) > 0)
                Request(p, 10);
            else
                Request(p, 18);
            break;
        case 6:
            Request(p, 2);
            break;
        case 7:
            Request(p, 15);
            break;
        case 5:
            Request(p, 13);
            break;
        case 9:
            Request(p, 14);
            break;
        case 8:
            Request(p, 1);
            return;
        default:
            return;
    }
}
