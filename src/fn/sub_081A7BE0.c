#include "global.h"

struct A {
    u8 f[0x5c7];
    u8 m;
};
void ScenePanel_AdvancePairState(void *);
void ScenePanel_AdvanceConditionalPairState(void *);

void Vanargand_UpdateModeEffect(struct A *p)
{
    switch (p->m) {
        case 1:
            ScenePanel_AdvancePairState(p);
            break;
        case 2:
        case 3:
            ScenePanel_AdvanceConditionalPairState(p);
            break;
    }
}
