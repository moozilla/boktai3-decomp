#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.3%
// Open: the two zero constants are materialized in the opposite order.
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
extern struct Vanargand *gUnk_02000250;
void sub_081A68CC(void *);
/* Registry routine 4D12: resets state fields of the Vanargand task
   (tentative; no options). */
void Vanargand_ResetFromScript(void)
{
    struct Vanargand *p = gUnk_02000250;
    u8 clear, delay, mode;
    u32 zero;
    u16 *field526 = &p->field526;
    zero = 0;
    clear = 0;
    *field526 = zero;
    delay = 14;
    p->fieldAC = clear;
    p->fieldAD = clear;
    p->fieldAE = delay;
    p->fieldC4 = zero;
    p->fieldB0 = 1;
    mode = 16;
    sub_081A68CC(p->block21C);
    p->fieldAA = mode;
}
#else
INCLUDE_ASM("asm/nonmatching", Vanargand_ResetFromScript);
#endif
