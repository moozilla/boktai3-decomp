#include "global.h"

struct A {
    u8 f[0x5c7];
    u8 m;
};
void VanargandPair_AdvancePairState(void *);
void VanargandPair_AdvanceConditionalPairState(void *);

void Vanargand_UpdateModeEffect(struct A *p)
{
    switch (p->m) {
        case 1:
            VanargandPair_AdvancePairState(p);
            break;
        case 2:
        case 3:
            VanargandPair_AdvanceConditionalPairState(p);
            break;
    }
}
