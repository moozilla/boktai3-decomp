#include "global.h"
struct DirectionalActor {
    u8 unknown0[0xB39];
    u8 direction;
    u8 unknownB3A[6];
    u8 phase, active, unknownB42, sequenceComplete;
};
extern const u32 gUnk_0824EDC8[];
void Armor_SetAnimationFromStart(struct DirectionalActor *, const u32 *);
void Armor_StartPaletteTransition(struct DirectionalActor *);
void DirectionalActor_UpdateSequence(struct DirectionalActor *p)
{
    u8 *s = &p->phase;
    u8 st = *s;
    switch (st) {
        case 0:
            Armor_SetAnimationFromStart(p, &gUnk_0824EDC8[((p->direction + 0x60) >> 6) & 3]);
            (*s)++;
            p->active = st;
            Armor_StartPaletteTransition(p);
            break;
        case 1:
            if (p->sequenceComplete != 0) {
                *s = 0xff;
                p->active = st;
            }
            break;
    }
}
