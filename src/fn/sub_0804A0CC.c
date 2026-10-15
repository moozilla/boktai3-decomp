#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(u8 *, u32);
u32 ScenePanel_IsPresent(u8 *);
u32 TetherActor_RequestKindNineAction(u32 a, u32 b)
{
    if (gUnk_02000488) {
        if (!ScenePanel_IsPresent(gUnk_02000488)) {
            if (gUnk_02000488[0x1A] == 9) {
                gUnk_02000488[0x240] = 1;
                gUnk_02000488[0x241] = b;
                *(u16 *)(gUnk_02000488 + 0x242) = a;
                return 1;
            }
        }
    }
    return 0;
}
