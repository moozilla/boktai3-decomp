#define ScenePanel_AdvancePairState sub_081A7A0C
#define ScenePair_CountPositiveCounters sub_081B658C
#include "global.h"
/* Tentative ScenePanel family; names describe observed state and sprite operations. */
struct ScenePanelPairState {
    u8 pad0[0x56C];
    void *pair;
    u8 pad570[0x5C4 - 0x570];
    u8 state;
};
s32 sub_081B658C(void *);
void sub_081A7A0C(struct ScenePanelPairState *panel)
{
    s32 count;
    switch (panel->state) {
    case 0:
        count = sub_081B658C(panel->pair);
        if (count == 2)
            panel->state = 1;
        else if (count == 0)
            panel->state = 2;
        break;
    case 2:
        count = sub_081B658C(panel->pair);
        if (count == 0) {
            panel->state = 12;
            break;
        }
        goto reset;
    case 11:
        panel->state = 2;
        break;
    case 1: case 3: case 4: case 5: case 6: case 7:
    case 8: case 9: case 10: case 12: default:
    reset:
        panel->state = 0;
        break;
    }
}
