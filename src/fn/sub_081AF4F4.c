#include "global.h"
#include "ram.h"
struct Vanargand {
    u8 pad0[0xAA];
    u8 fieldAA;
    u8 padAB;
    u8 fieldAC, fieldAD, fieldAE;
    u8 padAF;
    u8 fieldB0;
    u8 padB1[0x13];
    u32 fieldC4;
    u8 padC8[0x154];
    u8 block21C[0x30A];
    u16 field526;
};
void Vanargand_StopActionSequence(void *);
/* Registry routine 4D12: resets state fields of the Vanargand task
   (tentative; no options). */
/* Shared state change of the boss tasks (also used by 081803D8). */
static inline void setState(struct Vanargand *p, u8 state)
{
    p->fieldAC = 0;
    p->fieldAD = 0;
    p->fieldAE = state;
    p->fieldC4 = 0;
    p->fieldB0 = 1;
}
/* Registry routine 4D12: puts the Vanargand task back into state 14 (shared
   state fields +0xAC..+0xC4) and mode 16; clears +0x526 (tentative). */
void Vanargand_ResetFromScript(void)
{
    struct Vanargand *p = gSceneComposite;
    u8 mode;
    p->field526 = 0;
    setState(p, 14);
    mode = 16;
    Vanargand_StopActionSequence(p->block21C);
    p->fieldAA = mode;
}
