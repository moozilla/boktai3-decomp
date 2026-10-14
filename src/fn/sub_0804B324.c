#include "tether_actor.h"
s32 TetherActor_TestTerrainTransition(struct TetherActor *);
void TetherActor_BeginAuxiliaryState(struct TetherActor *);
u32 TetherActor_SelectNextState(struct TetherActor *p)
{
    if (p->kind == 9 && p->actionRequested != 0) {
        p->actionRequested = 0;
        return 0xb;
    }
    if (TetherActor_TestTerrainTransition(p) != 0 && p->kind != 9)
        TetherActor_BeginAuxiliaryState(p);
    switch (p->state) {
        case 0:
        case 3:
        case 4:
            if (gUnk_02000580[0]->tetherMode == 6)
                goto r1;
            break;
        case 1:
            if (gUnk_02000580[0]->tetherMode == 6) {
            r1:
                return 1;
            }
            return 0;
        case 0x10:
            if (gUnk_02000580[0]->tetherMode == 6)
                goto r11;
            break;
        case 0x11:
            if (gUnk_02000580[0]->tetherMode == 6) {
            r11:
                return 0x11;
            }
            return 0x10;
    }
    return p->state;
}
